#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

long long convertFromBase(const string& value, int base) {
    long long result = 0;
    long long power = 1;
    
    for (int i = value.length() - 1; i >= 0; i--) {
        int digit;
        if (value[i] >= '0' && value[i] <= '9') {
            digit = value[i] - '0';
        } else if (value[i] >= 'a' && value[i] <= 'z') {
            digit = value[i] - 'a' + 10;
        } else if (value[i] >= 'A' && value[i] <= 'Z') {
            digit = value[i] - 'A' + 10;
        } else {
            throw runtime_error("Invalid character in base conversion");
        }
        
        if (digit >= base) {
            throw runtime_error("Invalid digit for base");
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
        
        // Extract k
        size_t k_pos = content.find("\"k\"");
        size_t k_start = content.find(':', k_pos) + 1;
        while (k_start < content.length() && !isdigit(content[k_start])) k_start++;
        size_t k_end = k_start;
        while (k_end < content.length() && isdigit(content[k_end])) k_end++;
        int k = stoi(content.substr(k_start, k_end - k_start));
        

        map<int, pair<int, string>> points;
        for (int i = 1; i <= max(n, 15); i++) {  // Check up to 15
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
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


long long lagrangeInterpolation(const vector<pair<int, long long>>& points) {
    long long constant_term = 0;
    int k = points.size();
    
    for (int i = 0; i < k; i++) {
        long long xi = points[i].first;
        long long yi = points[i].second;
        
        // Calculate Lagrange basis polynomial Li(0)
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
        
        long long common_divisor = gcd(abs(numerator), abs(denominator));
        numerator /= common_divisor;
        denominator /= common_divisor;
        
  
        long long term = (yi * numerator) / denominator;
        constant_term += term;
    }
    
    return constant_term;
}

int main() {
    try {
        cout << "Processing Test Cases...\n\n";
        cout << "Processing Test Case 1...\n";
        tuple<int, int, map<int, pair<int, string>>> result1 = readTestCase("testcase1.json");
        int n1 = get<0>(result1);
        int k1 = get<1>(result1);
        map<int, pair<int, string>> points_data1 = get<2>(result1);
        

        vector<pair<int, long long>> points1;
        int count1 = 0;
        for (const auto& point : points_data1) {
            if (count1 >= k1) break;
            int x = point.first;
            int base = point.second.first;
            string value = point.second.second;
            long long y = convertFromBase(value, base);
            points1.push_back(make_pair(x, y));
            count1++;
        }
    
        long long secret1 = lagrangeInterpolation(points1);
        cout << "Secret for Test Case 1: " << secret1 << "\n\n";

        cout << "Processing Test Case 2...\n";
        tuple<int, int, map<int, pair<int, string>>> result2 = readTestCase("testcase2.json");
        int n2 = get<0>(result2);
        int k2 = get<1>(result2);
        map<int, pair<int, string>> points_data2 = get<2>(result2);
        
        // Decode points and use only first k points
        vector<pair<int, long long>> points2;
        int count2 = 0;
        for (const auto& point : points_data2) {
            if (count2 >= k2) break;
            int x = point.first;
            int base = point.second.first;
            string value = point.second.second;
            long long y = convertFromBase(value, base);
            points2.push_back(make_pair(x, y));
            count2++;
        }
        
        long long secret2 = lagrangeInterpolation(points2);
        cout << "Secret for Test Case 2: " << secret2 << "\n";
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}