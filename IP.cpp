#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

class BSTNode {
public:
    string ip;
    BSTNode* left;
    BSTNode* right;

    BSTNode(string ip) : ip(ip), left(nullptr), right(nullptr) {}
};

class BST {
private:
    BSTNode* root;

    BSTNode* insert(BSTNode* node, string ip) {
        if (!node) return new BSTNode(ip);
        if (ip < node->ip)
            node->left = insert(node->left, ip);
        else if (ip > node->ip)
            node->right = insert(node->right, ip);
        return node;
    }

    BSTNode* deleteNode(BSTNode* node, string ip) {
        if (!node) return node;

        if (ip < node->ip)
            node->left = deleteNode(node->left, ip);
        else if (ip > node->ip)
            node->right = deleteNode(node->right, ip);
        else {
            if (!node->left) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            BSTNode* temp = minValueNode(node->right);
            node->ip = temp->ip;
            node->right = deleteNode(node->right, temp->ip);
        }
        return node;
    }

    BSTNode* minValueNode(BSTNode* node) {
        BSTNode* current = node;
        while (current && current->left) {
            current = current->left;
        }
        return current;
    }

    bool search(BSTNode* node, string ip) {
        if (!node) return false;
        if (ip == node->ip) return true;
        if (ip < node->ip)
            return search(node->left, ip);
        return search(node->right, ip);
    }

    void rangeSearch(BSTNode* node, string low, string high, vector<string>& result) {
        if (!node) return;
        if (low < node->ip)
            rangeSearch(node->left, low, high, result);
        if (low <= node->ip && high >= node->ip)
            result.push_back(node->ip);
        if (high > node->ip)
            rangeSearch(node->right, low, high, result);
    }

    void freeMemory(BSTNode* node) {
        if (node) {
            freeMemory(node->left);
            freeMemory(node->right);
            delete node;
        }
    }

    void inOrderTraversal(BSTNode* node) {
        if (node) {
            inOrderTraversal(node->left);
            cout << node->ip << " ";
            inOrderTraversal(node->right);
        }
    }

    void saveToFile(BSTNode* node, ofstream& file) {
        if (node) {
            saveToFile(node->left, file);
            file << node->ip << endl;
            saveToFile(node->right, file);
        }
    }

    int countNodes(BSTNode* node) {
        if (!node) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

public:
    BST() : root(nullptr) {}
    ~BST() { freeMemory(root); }

    void insert(string ip) {
        root = insert(root, ip);
    }

    void deleteNode(string ip) {
        root = deleteNode(root, ip);
    }

    bool search(string ip) {
        return search(root, ip);
    }

    vector<string> rangeSearch(string low, string high) {
        vector<string> result;
        rangeSearch(root, low, high, result);
        return result;
    }

    void display() {
        inOrderTraversal(root);
        cout << endl;
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            saveToFile(root, file);
            file.close();
            cout << "Data saved to " << filename << endl;
        } else {
            cout << "Unable to open file " << filename << endl;
        }
    }

    int countNodes() {
        return countNodes(root);
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                insert(line);
            }
            file.close();
            cout << "Data loaded from " << filename << endl;
        } else {
            cout << "Unable to open file " << filename << endl;
        }
    }
};

class IPAddressManagementSystem {
private:
    BST bst;

    bool validateIP(const string& ip) {
        regex ipPattern(
            R"((\d{1,3}\.){3}\d{1,3})");
        if (!regex_match(ip, ipPattern)) return false;

        istringstream iss(ip);
        string segment;
        while (getline(iss, segment, '.')) {
            int value = stoi(segment);
            if (value < 0 || value > 255) return false;
        }
        return true;
    }

    string enterValidIP() {
        string ip;
        bool valid = false;
        while (!valid) {
            cout << "Enter IP address: ";
            cin >> ip;
            if (validateIP(ip)) {
                valid = true;
            } else {
                cout << "Invalid IP address format. Please try again.\n";
            }
        }
        return ip;
    }

public:
    void insertIP() {
        string ip = enterValidIP();
        if (!bst.search(ip)) {
            bst.insert(ip);
            cout << "IP " << ip << " inserted successfully.\n";
        } else {
            cout << "IP " << ip << " already exists.\n";
        }
    }

    void deleteIP() {
        string ip = enterValidIP();
        if (bst.search(ip)) {
            bst.deleteNode(ip);
            cout << "IP " << ip << " deleted successfully.\n";
        } else {
            cout << "IP " << ip << " not found.\n";
        }
    }

    void searchIP() {
        string ip = enterValidIP();
        if (bst.search(ip)) {
            cout << "IP " << ip << " found.\n";
        } else {
            cout << "IP " << ip << " not found.\n";
        }
    }

    void findIPsInRange() {
        string low, high;
        bool validLow = false, validHigh = false;
        do {
            cout << "Enter range (low and high) separated by space:\n";
            cin >> low >> high;
            validLow = validateIP(low);
            validHigh = validateIP(high);
            if (!validLow || !validHigh) {
                cout << "Invalid IP address format. Please try again.\n";
                // Clear input buffer to prevent infinite loop
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (!validLow || !validHigh);

        vector<string> result = bst.rangeSearch(low, high);
        if (result.empty()) {
            cout << "No IPs found in the range " << low << " to " << high << ".\n";
        } else {
            cout << "IPs in the range " << low << " to " << high << ":\n";
            for (const auto& ip : result) {
                cout << ip << "\n";
            }

            // Optional: Save results to a file
            cout << "Do you want to save the results to a file? (y/n): ";
            char saveOption;
            cin >> saveOption;
            if (saveOption == 'y' || saveOption == 'Y') {
                string filename;
                cout << "Enter filename: ";
                cin >> filename;
                ofstream outFile(filename);
                for (const auto& ip : result) {
                    outFile << ip << "\n";
                }
                outFile.close();
                cout << "Results saved to " << filename << "\n";
            }
        }
    }

    void displayAllIPs() {
        cout << "All IP addresses in the system:\n";
        bst.display();
    }

    void saveIPsToFile() {
        string filename;
        cout << "Enter filename to save IP addresses: ";
        cin >> filename;
        bst.saveToFile(filename);
    }

    void loadIPsFromFile() {
        string filename;
        cout << "Enter filename to load IP addresses: ";
        cin >> filename;
        bst.loadFromFile(filename);
    }

    void countTotalIPs() {
        cout << "Total number of IP addresses: " << bst.countNodes() << endl;
    }
};

int main() {
    IPAddressManagementSystem ipms;
    int choice;
    do {
        cout << "\nIP Address Management System\n";
        cout << "1. Insert IP address\n";
        cout << "2. Delete IP address\n";
        cout << "3. Search for IP address\n";
        cout << "4. Find IPs in a range\n";
        cout << "5. Display all IP addresses\n";
        cout << "6. Save IP addresses to file\n";
        cout << "7. Load IP addresses from file\n";
        cout << "8. Count total IP addresses\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                ipms.insertIP();
                break;
            case 2:
                ipms.deleteIP();
                break;
            case 3:
                ipms.searchIP();
                break;
            case 4:
                ipms.findIPsInRange();
                break;
            case 5:
                ipms.displayAllIPs();
                break;
            case 6:
                ipms.saveIPsToFile();
                break;
            case 7:
                ipms.loadIPsFromFile();
                break;
            case 8:
                ipms.countTotalIPs();
                break;
            case 9:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
    return 0;
}
