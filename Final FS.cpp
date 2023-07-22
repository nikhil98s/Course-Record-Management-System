#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

const int MIN_DEGREE = 3;


struct BTreeNode {
    bool leaf;
    int *keys;
    BTreeNode **child_pointers;
    int key_count;
    BTreeNode *parent;
};

BTreeNode* createNode(bool leaf) {
    BTreeNode* node = new BTreeNode;
    node->keys = new int[2 * MIN_DEGREE - 1];
    node->child_pointers = new BTreeNode*[2 * MIN_DEGREE];
    node->leaf = leaf;
    node->key_count = 0;
    node->parent = NULL;
    return node;
}

BTreeNode* searchKey(BTreeNode* node, int key) {
    int i = 0;
    while (i < node->key_count && key > node->keys[i])
        i++;
    if (node->keys[i] == key)
        return node;
    if (node->leaf)
        return NULL;
    return searchKey(node->child_pointers[i], key);
}

void splitChild(BTreeNode* parent, int index, BTreeNode* child) {
    BTreeNode* new_node = createNode(child->leaf);
    new_node->key_count = MIN_DEGREE - 1;
    for (int j = 0; j < MIN_DEGREE - 1; j++)
        new_node->keys[j] = child->keys[j + MIN_DEGREE];
    if (!child->leaf) {
        for (int j = 0; j < MIN_DEGREE; j++)
            new_node->child_pointers[j] = child->child_pointers[j + MIN_DEGREE];
    }
    child->key_count = MIN_DEGREE - 1;
    for (int j = parent->key_count; j >= index + 1; j--)
        parent->child_pointers[j + 1] = parent->child_pointers[j];
    parent->child_pointers[index + 1] = new_node;
    for (int j = parent->key_count - 1; j >= index; j--)
        parent->keys[j + 1] = parent->keys[j];
    parent->keys[index] = child->keys[MIN_DEGREE - 1];
    parent->key_count++;
}

void insertNonFull(BTreeNode* node, int key) {
    int i = node->key_count - 1;
    if (node->leaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->key_count++;
    } else {
        while (i >= 0 && key < node->keys[i])
            i--;
        i++;
        if (node->child_pointers[i]->key_count == (2 * MIN_DEGREE - 1)) {
            splitChild(node, i, node->child_pointers[i]);
            if (key > node->keys[i])
                i++;
        }
        insertNonFull(node->child_pointers[i], key);
    }
}

void displayInorder(BTreeNode* node) {
    int i;
    for (i = 0; i < node->key_count; i++) {
        if (!node->leaf)
            displayInorder(node->child_pointers[i]);
        std::cout << node->keys[i] << " ";
    }
    if (!node->leaf)
        displayInorder(node->child_pointers[i]);
}

void insertKey(BTreeNode* &root, int key) {
    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = key;
        root->key_count = 1;
    } else {
        if (root->key_count == (2 * MIN_DEGREE - 1)) {
            BTreeNode* new_root = createNode(false);
            new_root->child_pointers[0] = root;
            root->parent = new_root;
            splitChild(new_root, 0, root);
            insertNonFull(new_root, key);
            root = new_root;
        } else {
            insertNonFull(root, key);
        }
    }
}

bool validateAdminLogin(const std::string& username, const std::string& password) {
    std::string adminUsername = "admin";
    std::string adminPassword = "admin123";
    return (username == adminUsername && password == adminPassword);
}


void createRecord() {
    std::string studentName;
    std::string studentUSN;

    std::cout << "Enter the student name: ";
    std::getline(std::cin >> std::ws, studentName);

    std::cout << "Enter the student USN: ";
    std::getline(std::cin >> std::ws, studentUSN);


    std::ifstream file("courses.txt");
    std::string line;
    bool duplicateUSNFound = false;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string existingUSN;
        if (iss >> existingUSN) {
            if (existingUSN == studentUSN) {
                duplicateUSNFound = true;
                break;
            }
        }
    }
    file.close();

    if (duplicateUSNFound) {
        std::cout << "Duplicate USN found! Record not created." << std::endl;
        return;
    }

    std::cout << "Enter the number of courses opted: ";
    int numCourses;
    std::cin >> numCourses;

    std::set<int> courseIDs;
    bool duplicateCourseIDFound = false;

    std::ofstream outputFile("courses.txt", std::ios_base::app);
    for (int i = 0; i < numCourses; i++) {
        std::string courseName;
        int courseId;

        std::cout << "Enter the course ID for course " << i + 1 << ": ";
        std::cin >> courseId;

        if (courseIDs.count(courseId) > 0) {
            duplicateCourseIDFound = true;
            break;
        }

        courseIDs.insert(courseId);

        std::cout << "Enter the course name for course " << i + 1 << ": ";
        std::getline(std::cin >> std::ws, courseName);

        
        std::replace(courseName.begin(), courseName.end(), '\t', ' ');

        outputFile << studentUSN << '\t' << studentName << '\t' << courseId << '\t' << courseName << std::endl;
    }
    outputFile.close();

    if (duplicateCourseIDFound) {
        std::cout << "Duplicate course ID found for the same student! Record not created." << std::endl;
        std::ofstream("courses.txt", std::ios_base::app | std::ios_base::out | std::ios_base::trunc);
        return;
    }

    std::cout << "Record created successfully!" << std::endl;
}
void deleteRecord() {
    std::string deleteUSN;
    std::cout << "Enter the student USN to delete: ";
    std::getline(std::cin >> std::ws, deleteUSN);

    std::ifstream inputFile("courses.txt");
    std::ofstream outputFile("temp.txt");
    std::string line;
    bool found = false;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string studentUSN;
        std::string studentName;
        int courseId;
        std::string courseName;
        if (iss >> studentUSN >> studentName >> courseId >> courseName) {
            if (studentUSN != deleteUSN) {
                outputFile << line << std::endl;
            } else {
                found = true;
            }
        }
    }
    inputFile.close();
    outputFile.close();

    if (found) {
        std::remove("courses.txt");
        std::rename("temp.txt", "courses.txt");
        std::cout << "Record deleted successfully!" << std::endl;
    } else {
        std::remove("temp.txt");
        std::cout << "Student USN not found!" << std::endl;
    }
}

//void deleteRecordS(const std::string& loggedInUSN) {
//    std::ifstream inputFile("courses.txt");
//    std::ofstream outputFile("temp.txt");
//    std::string line;
//    bool found = false;
//
//    while (std::getline(inputFile, line)) {
//        std::istringstream iss(line);
//        std::string studentUSN;
//        std::string studentName;
//        int courseId;
//        std::string courseName;
//        if (iss >> studentUSN >> studentName >> courseId >> courseName) {
//            if (studentUSN == loggedInUSN) {
//                found = true;
//                std::cout << "Deleting record for student USN: " << studentUSN << std::endl;
//                std::cout << "Student Name: " << studentName << std::endl;
//            } else {
//                outputFile << line << std::endl; // Write unchanged record to output file
//            }
//        }
//    }
//    inputFile.close();
//    outputFile.close();
//
//    if (found) {
//        std::remove("courses.txt");
//        std::rename("temp.txt", "courses.txt");
//        std::cout << "Record deleted successfully!" << std::endl;
//    } else {
//        std::remove("temp.txt");
//        std::cout << "Student USN not found or you don't have permission to delete the record!" << std::endl;
//    }
//}



void viewRecords() {
    std::ifstream file("courses.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string studentUSN;
        std::string studentName;
        int courseId;
        std::string courseName;
        if (iss >> studentUSN >> studentName >> courseId >> courseName) {
            std::cout << "Student USN: " << studentUSN << "\tStudent Name: " << studentName
                      << "\tCourse ID: " << courseId << "\tCourse Name: " << courseName << std::endl;
        }
    }
    file.close();
}

void searchRecord() {
    std::string searchUSN;
    std::cout << "Enter the student USN to search: ";
    std::getline(std::cin >> std::ws, searchUSN);

    std::ifstream file("courses.txt");
    std::string line;
    bool found = false;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string studentUSN;
        std::string studentName;
        int courseId;
        std::string courseName;
        if (iss >> studentUSN >> studentName >> courseId >> courseName) {
            if (studentUSN == searchUSN) {
                found = true;
                std::cout << "Student USN: " << studentUSN << "\tStudent Name: " << studentName
                          << "\tCourse ID: " << courseId << "\tCourse Name: " << courseName << std::endl;
            }
        }
    }
    file.close();

    if (!found) {
        std::cout << "Student USN not found!" << std::endl;
    }
}

void updateRecord() {
    std::string updateUSN;
    std::cout << "Enter the student USN to update: ";
    std::getline(std::cin >> std::ws, updateUSN);

    std::ifstream inputFile("courses.txt");
    std::ofstream outputFile("temp.txt");
    std::string line;
    bool found = false;

    std::string studentUSN;
    std::string studentName;
    int courseId;
    std::string courseName;

    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        if (iss >> studentUSN >> studentName) {
            if (studentUSN == updateUSN) {
                found = true;
                std::cout << "Updating record for student USN: " << studentUSN << std::endl;
                std::cout << "Student Name: " << studentName << std::endl;

                std::vector<std::pair<int, std::string> > courses;
                int numCourses;

                std::cout << "Enter the number of courses to update: ";
                std::cin >> numCourses;
                std::cin.ignore();  

                
                while (iss >> courseId >> courseName) {
                    
                }

                
                for (int i = 0; i < numCourses; i++) {
                    std::cout << "Enter the course ID for course " << i + 1 << ": ";
                    std::cin >> courseId;

                    std::cout << "Enter the course name for course " << i + 1 << ": ";
                    std::cin.ignore();
                    std::getline(std::cin, courseName);

                    courses.push_back(std::make_pair(courseId, courseName));
                }

                
                for (std::vector<std::pair<int, std::string> >::iterator it = courses.begin(); it != courses.end(); ++it) {
                    outputFile << studentUSN << " " << studentName;
                    outputFile << " " << it->first << " " << it->second << std::endl;
                }
                outputFile << std::endl;
                break;
            } else {
                
                outputFile << line << std::endl;
            }
        }
    }
    inputFile.close();
    outputFile.close();

    if (found) {
        std::remove("courses.txt");
        std::rename("temp.txt", "courses.txt");
        std::cout << "Record updated successfully!" << std::endl;
        found = false;
    } else {
        std::remove("temp.txt");
        std::cout << "Student USN not found!" << std::endl;
    }
}



int main() {
    BTreeNode* root = NULL;

    std::ifstream file("courses.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int courseId;
        if (ss >> courseId)
            insertKey(root, courseId);
    }
    file.close();

    int choice;
    do {
        std::cout << "Login Options:\n";
        std::cout << "1. Admin Login\n";
        std::cout << "2. Student Menu\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string adminUsername, adminPassword;
                std::cout << "Admin Login\n";
                std::cout << "Username: ";
                std::cin >> adminUsername;
                std::cout << "Password: ";
                std::cin >> adminPassword;

                if (validateAdminLogin(adminUsername, adminPassword)) {
                    std::cout << "Admin login successful!\n";
                    int adminChoice;
                    do {
                        std::cout << "\nAdmin Menu:\n";
                        std::cout << "1. Create Student Record\n";
                        std::cout << "2. View Student Records\n";
                        std::cout << "3. Search Student Record\n";
                        std::cout << "4. Update Student Record\n";
                        std::cout << "5. Delete Student Record\n";
                        std::cout << "6. Logout\n";
                        std::cout << "Enter your choice: ";
                        std::cin >> adminChoice;

                        switch (adminChoice) {
                            case 1:
                                createRecord();
                                break;
                            case 2:
                                viewRecords();
                                break;
                            case 3:
                                searchRecord();
                                break;
                            case 4:
                                updateRecord();
                                break;
                            case 5:
                                deleteRecord();
                                break;
                            case 6:
                                std::cout << "Logged out!\n";
                                break;
                            default:

                                std::cout << "Invalid choice! Please try again.\n";
                                break;
                        }
                    } while (adminChoice != 6);
                } else {
                    std::cout << "Invalid login credentials!\n";
                }
                break;
            }
            case 2: {
                std::cout << "Student Menu\n";
                int studentChoice;
                std::string loggedInUSN;
                do {
                    std::cout << "\nStudent Menu:\n";
                    std::cout << "1. Create Student Record\n";
                    std::cout << "2. Logout\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> studentChoice;
                    switch (studentChoice) {
                        case 1:
                            createRecord();
                            break;
                        case 2:
                            std::cout << "Logged out!\n";
                            break;
                        default:
                            std::cout << "Invalid choice! Please try again.\n";
                            break;
                    }
                } while (studentChoice != 2);
                break;
            }
            case 3:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice! Please try again.\n";
                break;
        }
    } while (choice != 3);

    return 0;
}
