#include "DynamicTable.h"
#include "Ranking.h"

// ─── CSV Parsing Helpers ──────────────────────────────────────────────────────

/*
    Trims leading and trailing whitespace from a string.
    Example: "  Hello World  " -> "Hello World"
*/
static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

/*
    Removes trailing semicolons from a string.
    Example: "Hello World;;;" -> "Hello World"
*/
static string stripTrailingSemicolons(string s) {
    while (!s.empty() && s.back() == ';')
        s.pop_back();
    return s;
}

/*
    Removes the UTF-8 Byte Order Mark (BOM) from the beginning of a string.
*/
static string stripBOM(string s) {
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
        s.erase(0, 3);
    return s;
}

/*
    Fixes common UTF-8 encoding issues in movie titles by replacing known problematic substrings with their correct representations.
    This is a simple heuristic approach that targets specific encoding errors observed in the dataset.
    Example: "The Godfather â€ " -> "The Godfather –"
*/
static string fixEncoding(string s) {
    const char* bad[] = {" â€ ", " â€™ ", "Â˝", "Ă©", "Ă ", "Ă¼", "ĂĄ", "Ăł", "Ă"};
    const char* good[] = {" – ", " ' ", "½", "é", "à", "ü", "á", "ó", "Í"};
    
    for (int j = 0; j < 9; ++j) {
        size_t badLen = strlen(bad[j]);
        size_t goodLen = strlen(good[j]);
        size_t pos = 0;
        while ((pos = s.find(bad[j], pos)) != string::npos) {
            s.replace(pos, badLen, good[j]);
            pos += goodLen;
        }
    }
    return s;
}

/*
    Parses a single CSV line, correctly handling quoted fields with embedded commas.
    Returns a dynamically allocated array of strings; caller must delete[]
    Example: '123,"The Godfather, Part II",9' -> ["123", "The Godfather, Part II", "9"]
*/
static string* parseCSVLine(const string& line, int& fieldCount) {
    const int MAX_FIELDS = 256;
    string* fields = new string[MAX_FIELDS];
    fieldCount = 0;
    string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size() && fieldCount < MAX_FIELDS; ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields[fieldCount++] = trim(field);
            field.clear();
        } else {
            field += c;
        }
    }
    if (fieldCount < MAX_FIELDS) {
        fields[fieldCount++] = trim(field);
    }
    return fields;
}

/*
    Main catches 3 parameters: algorithm, size of data structure, path to data file.
    It fills the data structure with data from the file, then performs the tested operations.
    The object returned by this program is a string with 3 comma-separated values
    representing the time of each operation being tested.
*/

int main(int argc, char* argv[])
{
    /*
        Checking command line arguments.
            - The first argument specifies the sorting algorithm to use: "quick", "merge", or "intro".
            - The second argument specifies the number of records to read from the file and insert into the data structure.
            - The third argument is the path to the CSV data file containing movie ratings.
        Failure to provide the correct arguments will result in an error message and termination of the program.
    */
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <algorithm> <size> <data_file>" << endl;
        return 1;
    }

    /*
        Validating the sorting algorithm argument to ensure it is one of the accepted values ("quick", "merge", or "intro").
        If an invalid algorithm is specified, an error message is printed and the program exits with a non-zero code.
    */
    if (argv[1] != string("quick") && argv[1] != string("merge") && argv[1] != string("intro")) {
        cerr << "Invalid sorting algorithm specified. Use 'quick', 'merge', or 'intro'." << endl;
        return 1;
    }

    string              algo         = argv[1];
    int                 size         = stoi(argv[2]);
    string              data_file    = argv[3];

    ifstream infile(data_file);
    if (!infile) {
        cerr << "Error opening file: " << data_file << endl;
        return 1;                                                   // If file was not opened succesfully, print error message and exit with non-zero code
    }

    DynamicTable        table;
    int                 i{0};
    long long           sortTime{0};
    float               averageValue{0}, medianValue{0};
    long long           loadTime{0};
    bool                firstLine = true;

    /*
        Loop for loading data from .csv file into data structure, while measuring the time taken for this operation.
        The data cleaning and parsing is done in the loop, ensuring only valid entries are added.
    */
    auto loadStart = high_resolution_clock::now();
    while (i < size) {
        string line;
        if (!getline(infile, line)) break;

        if (firstLine) {
            line = stripBOM(line);
            firstLine = false;
            continue;
        }

        line = stripTrailingSemicolons(line);
        if (line.empty()) continue;

        int fieldCount = 0;
        string* fields = parseCSVLine(line, fieldCount);

        // At least 3 fields are required: ID, Title, and Rating
        constexpr int REQUIRED_FIELDS = 3;
        if (fieldCount < REQUIRED_FIELDS) { delete[] fields; continue; }

        string movieTitle = fields[1];
        // Only fix encoding if problematic substrings are present
        const char* encodingIssues[] = {"â€", "â€™", "Â˝", "Ă©", "Ă ", "Ă¼", "ĂĄ", "Ăł", "Ă"};
        for (const char* issue : encodingIssues) {
            if (movieTitle.find(issue) != string::npos) {
                movieTitle = fixEncoding(movieTitle);
                break;
            }
        }
        string rawRating  = fields[2];
        delete[] fields;

        if (movieTitle.empty() || rawRating.empty()) continue;

        int movieRating{0};

        /*
            Using try-catch block to handle potential exceptions from invalid rating values (e.g., non-numeric strings).
            If an exception occurs, the line is skipped and an error message is printed to stderr.
            This ensures that the program won't crash due to wrongly formatted or corrupted data.
        */
        try {
            movieRating = stoi(rawRating);
        } catch (const exception&) {
            cerr << "Skipping invalid rating at input line " << (i + 2) << ": " << line << endl;
            continue;
        }

        if (table.returnSize() > 0 && movieTitle == table.returnElementAtPosition(table.returnSize() - 1).getName()) {
            continue;
        }
        table.addElementAtEnd(Ranking(movieTitle, movieRating));
        // cout << "Added: " << movieTitle << " with rating " << movieRating << endl;
        i++;
    }
    auto loadEnd = high_resolution_clock::now();
    loadTime = duration_cast<nanoseconds>(loadEnd - loadStart).count(); // Time taken to load data into structure is saved for output
    infile.close();

    // ── Performing sorting operation and measuring time taken ─────────────────

    /*
        The sorting algorithm to be used is determined by the first command line argument.
        Depending on the specified algorithm, the corresponding sorting function is called on the data structure.
        The time taken for the sorting operation is measured and stored for output.
    */

    if(algo == "quick") {
        auto start = high_resolution_clock::now();
        table.quickSort(0, table.returnSize() - 1);
        auto end = high_resolution_clock::now();
        sortTime = duration_cast<nanoseconds>(end - start).count();
    }
    else if(algo == "merge") {
        auto start = high_resolution_clock::now();
        table.mergeSort(0, table.returnSize() - 1);
        auto end = high_resolution_clock::now();
        sortTime = duration_cast<nanoseconds>(end - start).count();
    }
    else if(algo == "intro") {
        int depthLimit = 2 * log(table.returnSize());
        auto start = high_resolution_clock::now();
        table.introSort(0, table.returnSize() - 1, depthLimit);
        auto end = high_resolution_clock::now();
        sortTime = duration_cast<nanoseconds>(end - start).count();
    }

    // ── Calculating average and median values ─────────────────────────────────

    /*
        Calculating the average and median values of the ratings in the data structure.
        The average is computed as the sum of all ratings divided by the number of entries.
        The median is found by sorting the ratings and selecting the middle value.
    */
    averageValue = table.findAverage();
    medianValue  = table.findMedian();

    // ── Print results  (format: loadTime, sortTime, averageValue, medianValue) ──────────

    /*
        Printing the results in the specified format: loadTime, sortTime, averageValue, medianValue.
    */
    cout << loadTime << ","
         << sortTime << ","
         << averageValue << ","
         << medianValue << endl;

    return 0;
}