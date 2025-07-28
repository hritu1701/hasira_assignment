#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <cctype>

using namespace std;

long long convertFromBase(const string& value, int base) {
    long long result = 0;
    long long power = 1;
    
    for (int i = value.length() - 1; i >= 0; i--) {
        int digit = 0;
        char c = value[i];
        
        if (isdigit(c)) {
            digit = c - '0';
        } else if (isalpha(c)) {
            if (isupper(c)) {
                digit = c - 'A' + 10;
            } else {
                digit = c - 'a' + 10;
            }
        } else {
            throw runtime_error("Invalid character");
        }
        
        if (digit >= base) {
            throw runtime_error("Invalid digit");
        }
        
        result += digit * power;
        power *= base;
    }
    
    return result;
}

class SimpleJSONParser {
public:
    static tuple<int, int, map<int, pair<int, string>>> parse(const string& content) {
        size_t n_pos = content.find("\"n\"");
        size_t n_start = content.find(':', n_pos) + 1;
        while (n_start < content.length() && !isdigit(content[n_start])) n_start++;
        size_t n_end = n_start;
        while (n_end < content.length() && isdigit(content[n_end])) n_end++;
        int n = stoi(content.substr(n_start, n_end - n_start));
        
        size_t k_pos = content.find("\"k\"");
        size_t k_start = content.find(':', k_pos) + 1;
        while (k_start < content.length() && !isdigit(content[k_start])) k_start++;
        size_t k_end = k_start;
        while (k_end < content.length() && isdigit(content[k_end])) k_end++;
        int k = stoi(content.substr(k_start, k_end - k_start));
        
        map<int, pair<int, string>> points;
        for (int i = 1; i <= max(n, 20); i++) {
            string key_pattern = "\"" + to_string(i) + "\"";
            size_t key_pos = content.find(key_pattern);
            if (key_pos == string::npos) continue;
            
            size_t base_pos = content.find("\"base\"", key_pos);
            size_t base_start = content.find(':', base_pos) + 1;
            while (base_start < content.length() && content[base_start] != '"') base_start++;
            base_start++;
            size_t base_end = base_start;
            while (base_end < content.length() && content[base_end] != '"') base_end++;
            string base_str = content.substr(base_start, base_end - base_start);
            int base = stoi(base_str);
            
            size_t value_pos = content.find("\"value\"", key_pos);
            size_t value_start = content.find(':', value_pos) + 1;
            while (value_start < content.length() && content[value_start] != '"') value_start++;
            value_start++;
            size_t value_end = value_start;
            while (value_end < content.length() && content[value_end] != '"') value_end++;
            string value = content.substr(value_start, value_end - value_start);
            
            points[i] = make_pair(base, value);
        }
        
        return make_tuple(n, k, points);
    }
};

tuple<int, int, map<int, pair<int, string>>> readTestCase(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filePath);
    }
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    return SimpleJSONParser::parse(content);
}

long long gcd(long long a, long long b) {
    a = abs(a);
    b = abs(b);
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

long long lagrangeInterpolation(const vector<pair<int, long long>>& points) {
    if (points.size() < 2) {
        throw runtime_error("Need at least 2 points for interpolation");
    }
    
    long long constant_term = 0;
    int k = points.size();
    
    for (int i = 0; i < k; i++) {
        long long xi = points[i].first;
        long long yi = points[i].second;
        
        long long numerator = 1;
        long long denominator = 1;
        
        for (int j = 0; j < k; j++) {
            if (i != j) {
                long long xj = points[j].first;
                numerator *= (0 - xj);
                denominator *= (xi - xj);
            }
        }
        
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        
        if (denominator != 0) {
            long long common_divisor = gcd(numerator, denominator);
            numerator /= common_divisor;
            denominator /= common_divisor;
            
            if (denominator != 0) {
                long long term = (yi * numerator) / denominator;
                constant_term += term;
            }
        }
    }
    
    return constant_term;
}

vector<pair<int, long long>> findValidShares(
    const vector<pair<int, long long>>& allShares, 
    int requiredShares) {
    
    if (allShares.size() < requiredShares) {
        throw runtime_error("Not enough shares provided");
    }
    
    vector<bool> v(allShares.size());
    fill(v.end() - requiredShares, v.end(), true);
    
    map<long long, int> resultCount;
    vector<pair<vector<pair<int, long long>>, long long>> validCombinations;
    
    do {
        vector<pair<int, long long>> subset;
        for (int i = 0; i < allShares.size(); ++i) {
            if (v[i]) {
                subset.push_back(allShares[i]);
            }
        }
        
        if (subset.size() == requiredShares) {
            try {
                long long result = lagrangeInterpolation(subset);
                resultCount[result]++;
                validCombinations.push_back({subset, result});
            } catch (...) {
            }
        }
    } while (next_permutation(v.begin(), v.end()));
    
    long long mostCommonResult = 0;
    int maxCount = 0;
    for (const auto& pair : resultCount) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostCommonResult = pair.first;
        }
    }
    
    for (const auto& combo : validCombinations) {
        if (combo.second == mostCommonResult) {
            return combo.first;
        }
    }
    
    if (!validCombinations.empty()) {
        return validCombinations[0].first;
    }
    
    throw runtime_error("No valid combination of shares found");
}

int main() {
    try {
        tuple<int, int, map<int, pair<int, string>>> result1 = readTestCase("testcase1.json");
        int k1 = get<1>(result1);
        map<int, pair<int, string>> points_data1 = get<2>(result1);
        
        vector<pair<int, long long>> allPoints1;
        for (const auto& point : points_data1) {
            int x = point.first;
            int base = point.second.first;
            string value = point.second.second;
            try {
                long long y = convertFromBase(value, base);
                allPoints1.push_back(make_pair(x, y));
            } catch (const exception& e) {
            }
        }
        
        if (allPoints1.size() >= k1) {
            vector<pair<int, long long>> validShares1 = findValidShares(allPoints1, k1);
            long long secret1 = lagrangeInterpolation(validShares1);
            cout << "Secret for Test Case 1: " << secret1 << "\n";
        }
        
        tuple<int, int, map<int, pair<int, string>>> result2 = readTestCase("testcase2.json");
        int k2 = get<1>(result2);
        map<int, pair<int, string>> points_data2 = get<2>(result2);
        
        vector<pair<int, long long>> allPoints2;
        for (const auto& point : points_data2) {
            int x = point.first;
            int base = point.second.first;
            string value = point.second.second;
            try {
                long long y = convertFromBase(value, base);
                allPoints2.push_back(make_pair(x, y));
            } catch (const exception& e) {
            }
        }
        
        if (allPoints2.size() >= k2) {
            vector<pair<int, long long>> validShares2 = findValidShares(allPoints2, k2);
            long long secret2 = lagrangeInterpolation(validShares2);
            cout << "Secret for Test Case 2: " << secret2 << "\n";
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}