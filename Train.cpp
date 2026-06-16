/*
    THE IRON NEXUS - Railway Management System
    Module 1: Train Registry      - AVL Tree
    Module 2: Coach Management    - Doubly Linked List
    Module 3: Railway Track Network - Weighted Graph + Dijkstra
    Module 4: Seat Booking        - Hash Table + BST
    Module 5: Operation Logging   - Stack
*/
#pragma once
#define TRAIN_H

#include <iostream>
#include <string>
#include <fstream>
#include <climits>
#include "Classes.h"

using namespace std;

const int TableSize = 100;


struct Coach {
    int id;
    string name;
    Coach* next;
    Coach* prev;

    Coach() {
        id = 0;
        name = "";
        next = prev = nullptr;
    }

    Coach(int i, string n) {
        id = i;
        name = n;
        next = prev = nullptr;
    }
};


class CoachList {
private:
    Coach* head;
    Coach* tail;
    int    coachCount;

public:

    CoachList() {
        head = tail = nullptr;
        coachCount = 0;
    }

    ~CoachList() {
        Coach* curr = head;
        while (curr) {
            Coach* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void addAtFront(int id, string name, Stack& logger, ActionStack& undoStack) {
        Coach* newCoach = new Coach(id, name);
        if (!head) {
            head = tail = newCoach;
        }
        else {
            newCoach->next = head;
            head->prev = newCoach;
            head = newCoach;
        }
        coachCount++;
        printSuccess("Coach [" + to_string(id) + "] " + name + " added at front.");
        logger.push("Coach [" + to_string(id) + "] " + name + " added at front.");
        undoStack.push({ "ADD_COACH_FRONT", to_string(id) + "|" + name });
    }

    void addAtEnd(int id, string name, Stack& logger, ActionStack& undoStack) {
        Coach* newCoach = new Coach(id, name);
        if (!tail) {
            head = tail = newCoach;
        }
        else {
            tail->next = newCoach;
            newCoach->prev = tail;
            tail = newCoach;
        }
        coachCount++;
        printSuccess("Coach [" + to_string(id) + "] " + name + " added at end.");
        logger.push("Coach [" + to_string(id) + "] " + name + " added at end.");
        undoStack.push({ "ADD_COACH_END", to_string(id) + "|" + name });
    }

    void addAfter(int afterId, int id, string name, Stack& logger, ActionStack& undoStack) {
        Coach* curr = head;
        while (curr) {
            if (curr->id == afterId) {
                Coach* newCoach = new Coach(id, name);
                newCoach->next = curr->next;
                newCoach->prev = curr;
                if (curr->next) {
                    curr->next->prev = newCoach;
                }
                else {
                    tail = newCoach;
                }
                curr->next = newCoach;
                coachCount++;
                printSuccess("Coach [" + to_string(id) + "] " + name + " inserted after coach " + to_string(afterId) + ".");
                logger.push("Coach [" + to_string(id) + "] " + name + " inserted after coach " + to_string(afterId) + ".");
                undoStack.push({ "ADD_COACH_AFTER", to_string(id) + "|" + name + "|" + to_string(afterId) });
                return;
            }
            curr = curr->next;
        }
        printError("Coach " + to_string(afterId) + " not found. Insertion failed.");
    }

    void removeCoach(int id, Stack& logger, ActionStack& undoStack) {
        Coach* curr = head;
        while (curr) {
            if (curr->id == id) {
                string savedName = curr->name;
                if (curr->prev) {
                    curr->prev->next = curr->next;
                }
                else {
                    head = curr->next;
                }
                if (curr->next) {
                    curr->next->prev = curr->prev;
                }
                else {
                    tail = curr->prev;
                }
                delete curr;
                coachCount--;
                printSuccess("Coach [" + to_string(id) + "] removed for maintenance.");
                logger.push("Coach [" + to_string(id) + "] removed for maintenance.");
                undoStack.push({ "REMOVE_COACH", to_string(id) + "|" + savedName });
                return;
            }
            curr = curr->next;
        }
        printError("Coach " + to_string(id) + " not found.");
    }

    void traverseForward() const {
        string chain = "";
        Coach* curr = head;
        while (curr) {
            chain += FG_BBLUE "->" CLR_RESET;
            chain += " [";
            chain += FG_BCYAN + to_string(curr->id) + CLR_RESET;
            chain += "] ";
            chain += FG_BWHITE + curr->name + CLR_RESET;
            chain += " ";
            curr = curr->next;
        }
        chain += FG_BBLUE "->" CLR_RESET;
        cout << "  " FG_BBLUE "Engine " CLR_RESET << chain << " " FG_BBLUE "Caboose" CLR_RESET "\n";
    }

    void traverseBackward() const {
        string chain = "";
        Coach* curr = tail;
        while (curr) {
            chain += FG_BBLUE "<-" CLR_RESET;
            chain += " [";
            chain += FG_BCYAN + to_string(curr->id) + CLR_RESET;
            chain += "] ";
            chain += FG_BWHITE + curr->name + CLR_RESET;
            chain += " ";
            curr = curr->prev;
        }
        chain += FG_BBLUE "<-" CLR_RESET;
        cout << "  " FG_BBLUE "Caboose " CLR_RESET << chain << " " FG_BBLUE "Engine" CLR_RESET "\n";
    }

    void reverse(Stack& logger, ActionStack& undoStack) {
        Coach* curr = head;
        Coach* temp = nullptr;
        while (curr) {
            temp = curr->prev;
            curr->prev = curr->next;
            curr->next = temp;
            curr = curr->prev;
        }
        if (temp) {
            head = temp->prev;
        }
        temp = head;
        head = tail;
        tail = temp;
        printSuccess("Train orientation reversed.");
        logger.push("Train orientation reversed.");
        undoStack.push({ "REVERSE_COACHES", "" });
    }

    int totalCoaches() const {
        return coachCount;
    }

    Coach* getHead() const {
        return head;
    }
};


struct Station {
    int id;
    string name;
    bool isActive;

    Station() {
        id = 0;
        name = "";
        isActive = false;
    }

    Station(int i, string n, bool a) {
        id = i;
        name = n;
        isActive = a;
    }
};


class RailNetwork {
private:
    Station stations[15];
    int adj[15][15];
    int stationcount;

    int getIndex(int id) {
        for (int i = 0; i < stationcount; i++) {
            if (stations[i].id == id) {
                return i;
            }
        }
        return -1;
    }

public:
    RailNetwork() {
        stationcount = 0;
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j++) {
                adj[i][j] = 0;
            }
        }
    }

    void addStation(int id, string name, Stack& logger, ActionStack& undoStack) {
        if (stationcount >= 15) {
            printError("Network full. Cannot add more stations.");
            return;
        }
        stations[stationcount].id = id;
        stations[stationcount].name = name;
        stations[stationcount].isActive = true;
        stationcount++;
        printSuccess("Station [" + to_string(id) + "] " + name + " added.");
        logger.push("Station [" + to_string(id) + "] " + name + " added to network.");
        undoStack.push({ "ADD_STATION", to_string(id) + "|" + name });
    }

    void removeStation(int id, Stack& logger, ActionStack& undoStack) {
        int idx = getIndex(id);
        if (idx == -1) {
            printError("Station " + to_string(id) + " not found.");
            return;
        }
        string savedName = stations[idx].name;
        stations[idx].isActive = false;
        for (int j = 0; j < stationcount; j++) {
            adj[idx][j] = 0;
            adj[j][idx] = 0;
        }
        printSuccess("Station [" + to_string(id) + "] " + savedName + " removed.");
        logger.push("Station [" + to_string(id) + "] " + savedName + " removed from network.");
        undoStack.push({ "REMOVE_STATION", to_string(id) + "|" + savedName });
    }

    void addTrack(int fromId, int toId, int weight, Stack& logger, ActionStack& undoStack) {
        int i = getIndex(fromId);
        int j = getIndex(toId);
        if (i == -1 || j == -1) {
            printError("One or both stations not found.");
            return;
        }
        adj[i][j] = weight;
        adj[j][i] = weight;
        printSuccess("Track [" + to_string(fromId) + "] <-> [" + to_string(toId) + "] | Distance: " + to_string(weight));
        logger.push("Track added: [" + to_string(fromId) + "] <-> [" + to_string(toId) + "] | Distance: " + to_string(weight));
        undoStack.push({ "ADD_TRACK", to_string(fromId) + "|" + to_string(toId) + "|" + to_string(weight) });
    }

    void removeTrack(int fromId, int toId, Stack& logger, ActionStack& undoStack) {
        int i = getIndex(fromId);
        int j = getIndex(toId);
        if (i == -1 || j == -1) {
            printError("One or both stations not found.");
            return;
        }
        int savedWeight = adj[i][j];
        adj[i][j] = 0;
        adj[j][i] = 0;
        printSuccess("Track removed between [" + to_string(fromId) + "] and [" + to_string(toId) + "].");
        logger.push("Track removed between [" + to_string(fromId) + "] and [" + to_string(toId) + "].");
        undoStack.push({ "REMOVE_TRACK", to_string(fromId) + "|" + to_string(toId) + "|" + to_string(savedWeight) });
    }

    void reactivateStation(int id) {
        for (int i = 0; i < stationcount; i++) {
            if (stations[i].id == id) {
                stations[i].isActive = true;
                return;
            }
        }
    }

    void setTrack(int fromId, int toId, int weight) {
        int i = getIndex(fromId);
        int j = getIndex(toId);
        if (i != -1 && j != -1) {
            adj[i][j] = weight;
            adj[j][i] = weight;
        }
    }

    void shortestPath(int startId, int goalId) {
        int start = getIndex(startId);
        int goal = getIndex(goalId);

        if (start == -1 || goal == -1) {
            printError("One or both stations not found.");
            return;
        }

        bool explored[15] = { false };
        int  path[15];
        int  prev[15];

        for (int i = 0; i < stationcount; i++) {
            path[i] = INT_MAX;
            prev[i] = -1;
        }

        PriorityQueue pq;

        path[start] = 0;
        pq.push(start, 0);

        while (!pq.isEmpty()) {
            PQNode current = pq.pop();
            int u = current.vertex;

            if (explored[u]) continue;
            explored[u] = true;

            if (u == goal || path[u] == INT_MAX) break;

            for (int v = 0; v < stationcount; v++) {
                if (adj[u][v] != 0 && !explored[v] && stations[v].isActive) {
                    if (path[u] + adj[u][v] < path[v]) {
                        path[v] = path[u] + adj[u][v];
                        prev[v] = u;
                        pq.push(v, path[v]);
                    }
                }
            }
        }

        printSubTitle("Shortest Path Result (PQ Optimized)");
        if (path[goal] == INT_MAX) {
            printError("No path found between " + stations[start].name + " and " + stations[goal].name + ".");
            printLine();
            return;
        }

        printRow("From", stations[start].name);
        printRow("To", stations[goal].name);
        printRow("Distance", to_string(path[goal]) + " km");

        int route[15];
        int routeLen = 0;
        for (int at = goal; at != -1; at = prev[at]) {
            route[routeLen++] = at;
        }
        cout << FG_CYAN "| " CLR_RESET FG_BWHITE "Route: " CLR_RESET;
        for (int i = routeLen - 1; i >= 0; i--) {
            cout << FG_BGREEN << stations[route[i]].name << CLR_RESET;
            if (i != 0) cout << FG_BYELLOW " -> " CLR_RESET;
        }
        cout << "\n";
        printLine();
    }



    void displayNetwork() const {
        printSubTitle("Rail Network Stations");
        for (int i = 0; i < stationcount; i++) {
            string status = stations[i].isActive
                ? FG_BGREEN "Active"   CLR_RESET
                : FG_BRED   "Inactive" CLR_RESET;
            cout << FG_CYAN "| " CLR_RESET;
            cout << FG_BYELLOW "[" << stations[i].id << "]" CLR_RESET " ";
            cout << FG_BWHITE << stations[i].name << CLR_RESET;
            int used = 2 + 3 + (int)stations[i].name.size() + 1;
            for (int s = used; s < 44; s++) cout << " ";
            cout << status;

            cout << "        " FG_CYAN "|" CLR_RESET "\n";
        }
        printLine();
    }

    void saveToFile() {
        ofstream file("network.txt");
        if (!file) {
            printError("Could not open network.txt for writing.");
            return;
        }
        file << stationcount << "\n";
        for (int i = 0; i < stationcount; i++) {
            file << stations[i].id << " " << stations[i].isActive << " " << stations[i].name << "\n";
        }
        for (int i = 0; i < stationcount; i++) {
            for (int j = 0; j < stationcount; j++) {
                file << adj[i][j];
                if (j < stationcount - 1) file << " ";
            }
            file << "\n";
        }
        file.close();
        printSuccess("Network saved to network.txt");
    }

    void loadFromFile() {
        ifstream file("network.txt");
        if (!file) {
            printError("network.txt not found.");
            return;
        }
        file >> stationcount;
        file.ignore();
        for (int i = 0; i < stationcount; i++) {
            file >> stations[i].id >> stations[i].isActive;
            file.ignore();
            getline(file, stations[i].name);
        }
        for (int i = 0; i < stationcount; i++) {
            for (int j = 0; j < stationcount; j++) {
                file >> adj[i][j];
            }
        }
        file.close();
        printSuccess("Network loaded from network.txt");
    }
};


//  Module 4: Seat BST

struct Seat {
    int seatno;
    bool booked;
    string passengername;
    Seat* left;
    Seat* right;

    Seat() {
        seatno = 0;
        booked = false;
        passengername = "";
        left = right = NULL;
    }
};


class SeatIndex {
private:
    Seat* root;

    Seat* createNode(int seatno) {
        Seat* temp = new Seat;
        temp->seatno = seatno;
        temp->booked = false;
        temp->left = NULL;
        temp->right = NULL;
        return temp;
    }

    Seat* insert(Seat* node, int seatno) {
        if (node == NULL) {
            return createNode(seatno);
        }
        if (seatno < node->seatno) {
            node->left = insert(node->left, seatno);
        }
        else if (seatno > node->seatno) {
            node->right = insert(node->right, seatno);
        }
        return node;
    }


    Seat* deleteNode(Seat* root, int seatno) {
        if (!root) return nullptr;
        if (seatno < root->seatno)
            root->left = deleteNode(root->left, seatno);
        else if (seatno > root->seatno)
            root->right = deleteNode(root->right, seatno);
        else {
            if (!root->left) { Seat* t = root->right; delete root; return t; }
            if (!root->right) { Seat* t = root->left; delete root; return t; }
            Seat* succ = root->right;
            while (succ->left) succ = succ->left;
            root->seatno = succ->seatno;
            root->booked = succ->booked;
            root->passengername = succ->passengername;
            root->right = deleteNode(root->right, succ->seatno);
        }
        return root;
    }

    void deleteSeat(int seatno) {
        root = deleteNode(root, seatno);
    }


    Seat* search(Seat* node, int seatno) {
        if (node == NULL || node->seatno == seatno) {
            return node;
        }
        if (seatno < node->seatno) {
            return search(node->left, seatno);
        }
        else {
            return search(node->right, seatno);
        }
    }

    void inorderBooked(Seat* node) {
        if (node == NULL) return;
        inorderBooked(node->left);
        if (node->booked == true) {
            cout << FG_CYAN "| " CLR_RESET;
            cout << FG_BYELLOW "Seat " << node->seatno << CLR_RESET;
            cout << FG_WHITE " : " CLR_RESET;
            cout << FG_BRED "Booked by " CLR_RESET;
            cout << FG_BWHITE << node->passengername << CLR_RESET;
            int used = 2 + 5 + (int)to_string(node->seatno).size() + 3 + 10 + (int)node->passengername.size();
            for (int i = used; i < 51; i++) cout << " ";
            cout << FG_CYAN "|" CLR_RESET "\n";
        }
        inorderBooked(node->right);
    }

    void inorderAvailable(Seat* node) {
        if (node == NULL) return;
        inorderAvailable(node->left);
        if (node->booked == false) {
            cout << FG_CYAN "| " CLR_RESET;
            cout << FG_BYELLOW "Seat " << node->seatno << CLR_RESET;
            cout << FG_WHITE " : " CLR_RESET;
            cout << FG_BGREEN "Available" CLR_RESET;
            int used = 2 + 5 + (int)to_string(node->seatno).size() + 3 + 9;
            for (int i = used; i < 51; i++) cout << " ";
            cout << FG_CYAN "|" CLR_RESET "\n";
        }
        inorderAvailable(node->right);
    }

    void inorder(Seat* node) {
        if (node == NULL) return;
        inorder(node->left);
        cout << FG_CYAN "| " CLR_RESET;
        cout << FG_BYELLOW "Seat " << node->seatno << CLR_RESET;
        cout << FG_WHITE " : " CLR_RESET;
        if (node->booked) {
            cout << FG_BRED "Booked by: " CLR_RESET FG_BWHITE << node->passengername << CLR_RESET;
            int used = 2 + 5 + (int)to_string(node->seatno).size() + 3 + 11 + (int)node->passengername.size();
            for (int i = used; i < 51; i++) cout << " ";
        }
        else {
            cout << FG_BGREEN "Available" CLR_RESET;
            int used = 2 + 5 + (int)to_string(node->seatno).size() + 3 + 9;
            for (int i = used; i < 51; i++) cout << " ";
        }
        cout << FG_CYAN "|" CLR_RESET "\n";
        inorder(node->right);
    }

    void savePreOrder(Seat* node, ofstream& file, int depth = 0) {
        string indentation = "";
        for (int i = 0; i < depth; i++) {
            indentation += "    ";
        }

        if (node == NULL) {
            file << indentation << "NULL\n";
            return;
        }

        file << indentation << node->seatno << " " << node->booked << " " << node->passengername << "\n";

        savePreOrder(node->left, file, depth + 1);
        savePreOrder(node->right, file, depth + 1);
    }


    Seat* loadPreOrder(ifstream& file) {
        string line;
        if (!getline(file, line)) return NULL;


        int startIdx = 0;
        while (startIdx < line.length() && (line[startIdx] == ' ' || line[startIdx] == '\t')) {
            startIdx++;
        }
        string cleanLine = line.substr(startIdx);

        if (cleanLine == "NULL" || cleanLine.empty()) return NULL;

        Seat* node = new Seat;
        int spaceOne = cleanLine.find(' ');
        int spaceTwo = cleanLine.find(' ', spaceOne + 1);

        node->seatno = stoi(cleanLine.substr(0, spaceOne));
        node->booked = stoi(cleanLine.substr(spaceOne + 1, spaceTwo - spaceOne - 1));
        node->passengername = cleanLine.substr(spaceTwo + 1);

        node->left = loadPreOrder(file);
        node->right = loadPreOrder(file);
        return node;
    }

public:
    SeatIndex() {
        root = NULL;
    }

    void insertseat(int seatno) {
        root = insert(root, seatno);
    }

    void updatestatus(int seatno, bool status, string name) {
        Seat* temp = search(root, seatno);
        if (temp != NULL) {
            temp->booked = status;
            temp->passengername = name;
        }
    }

    void displayBookedSeats() {
        printSubTitle("Booked Seats");
        inorderBooked(root);
        printLine();
    }

    void displayAvailableSeats() {
        printSubTitle("Available Seats");
        inorderAvailable(root);
        printLine();
    }

    void display() {
        printSubTitle("All Seats Sorted");
        inorder(root);
        printLine();
    }

    bool exists(int seatNo) {
        Seat* temp = search(root, seatNo);
        return temp != NULL;
    }

    // Bonus: save tree structure preserving hierarchy
    void saveTreeStructure() {
        ofstream file("seats_tree.txt");
        if (!file) {
            printError("Could not open seats_tree.txt");
            return;
        }
        savePreOrder(root, file);
        file.close();
        printSuccess("BST tree structure saved to seats_tree.txt");
    }

    // Bonus: load tree preserving hierarchy
    void loadTreeStructure() {
        ifstream file("seats_tree.txt");
        if (!file) {
            printError("seats_tree.txt not found.");
            return;
        }
        root = loadPreOrder(file);
        file.close();
        printSuccess("BST tree structure loaded from seats_tree.txt");
    }
};


//  Module 4: Hash Table


class HashTable {
public:
    struct SeatNode {
        int seatNumber;
        bool booked;
        string passengername;
        SeatNode* next;
    };

    SeatNode* table[TableSize];

    HashTable() {
        for (int i = 0; i < TableSize; i++) {
            table[i] = NULL;
        }
    }

    int hashFunction(int seatNumber) {
        return seatNumber % TableSize;
    }

    void insert(int seatNumber) {
        int index = hashFunction(seatNumber);
        SeatNode* newNode = new SeatNode;
        newNode->seatNumber = seatNumber;
        newNode->booked = false;
        newNode->next = NULL;

        if (table[index] == NULL) {
            table[index] = newNode;
        }
        else {
            SeatNode* temp = table[index];
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    SeatNode* search(int seatNumber) {
        int index = hashFunction(seatNumber);
        SeatNode* temp = table[index];
        while (temp != NULL) {
            if (temp->seatNumber == seatNumber) {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    void remove(int seatNumber) {
        int index = hashFunction(seatNumber);
        SeatNode* temp = table[index];
        SeatNode* prev = NULL;

        while (temp != NULL) {
            if (temp->seatNumber == seatNumber) {
                if (prev == NULL) {
                    table[index] = temp->next;
                }
                else {
                    prev->next = temp->next;
                }
                delete temp;
                printSuccess("Seat removed.");
                return;
            }
            prev = temp;
            temp = temp->next;
        }
        printError("Seat not found.");
    }

    void checkSeat(int seatNumber) {
        SeatNode* temp = search(seatNumber);
        if (temp == NULL) {
            printError("Seat not found.");
            return;
        }
        if (temp->booked == true) {
            printInfo("Seat " + to_string(seatNumber) + " is booked.");
        }
        else {
            printInfo("Seat " + to_string(seatNumber) + " is available.");
        }
    }
};


//  Module 4: Seating Chart


class SeatingChart {
private:
    int totalSeats;
    HashTable hashTable;
    SeatIndex bst;
    bool initialized;

public:

    SeatingChart() {
        totalSeats = 0;
        initialized = false;
    }

    void initializeSeats(int seats, Stack& logger) {
        initialized = true;
        totalSeats = seats;
        for (int i = 1; i <= totalSeats; i++) {
            hashTable.insert(i);
            bst.insertseat(i);
        }
        printSuccess("Seats initialized successfully.");
        logger.push("Seating chart initialized with " + to_string(seats) + " seats.");
    }

    void bookSeat(int seatNo, string name, Stack& logger, ActionStack& undoStack) {
        if (!initialized) {
            printError("Please initialize seats first.");
            return;
        }
        if (seatNo <= 0) {
            printError("Invalid seat number.");
            return;
        }
        if (seatNo > totalSeats) {
            printError("Seat number out of range.");
            return;
        }

        HashTable::SeatNode* h = hashTable.search(seatNo);
        if (h == NULL) {
            printError("Seat does not exist.");
            return;
        }
        if (h->booked == true) {
            printError("Seat " + to_string(seatNo) + " already booked by " + h->passengername + ".");
            return;
        }

        h->booked = true;
        h->passengername = name;
        bst.updatestatus(seatNo, true, name);
        printSuccess("Seat [" + to_string(seatNo) + "] booked for " + name + ".");
        logger.push("Seat [" + to_string(seatNo) + "] booked for " + name + ".");
        undoStack.push({ "BOOK_SEAT", to_string(seatNo) + "|" + name });
    }

    void cancelSeat(int seatNo, Stack& logger, ActionStack& undoStack) {
        if (!initialized) {
            printError("Please initialize seats first.");
            return;
        }
        if (seatNo <= 0) {
            printError("Invalid seat number.");
            return;
        }
        if (seatNo > totalSeats) {
            printError("Seat number out of range.");
            return;
        }

        HashTable::SeatNode* h = hashTable.search(seatNo);
        if (h == NULL) {
            printError("Seat does not exist.");
            return;
        }
        if (h->booked == false) {
            printError("Seat " + to_string(seatNo) + " is already available.");
            return;
        }

        string savedName = h->passengername;
        h->booked = false;
        h->passengername = "";
        bst.updatestatus(seatNo, false, "");
        printSuccess("Booking for seat [" + to_string(seatNo) + "] cancelled.");
        logger.push("Seat [" + to_string(seatNo) + "] booking cancelled.");
        undoStack.push({ "CANCEL_SEAT", to_string(seatNo) + "|" + savedName });
    }


    void directBook(int seatNo, string name) {
        HashTable::SeatNode* h = hashTable.search(seatNo);
        if (h && !h->booked) {
            h->booked = true;
            h->passengername = name;
            bst.updatestatus(seatNo, true, name);
        }
    }

    void directCancel(int seatNo) {
        HashTable::SeatNode* h = hashTable.search(seatNo);
        if (h && h->booked) {
            h->booked = false;
            h->passengername = "";
            bst.updatestatus(seatNo, false, "");
        }
    }

    void checkSeat(int seatNo) {
        if (!initialized) { printError("Please initialize seats first."); return; }
        if (seatNo <= 0) { printError("Invalid seat number."); return; }
        if (seatNo > totalSeats) { printError("Seat number out of range."); return; }

        HashTable::SeatNode* h = hashTable.search(seatNo);
        if (h == NULL) { printError("Seat does not exist."); return; }

        printSubTitle("Seat Status");
        if (h->booked) {
            printRow("Seat " + to_string(seatNo), FG_BRED "Booked by " CLR_RESET + h->passengername);
        }
        else {
            printRow("Seat " + to_string(seatNo), FG_BGREEN "Available" CLR_RESET);
        }
        printLine();
    }

    void displaySorted() { bst.display(); }
    void displayBookedSeats() { bst.displayBookedSeats(); }
    void displayAvailableSeats() { bst.displayAvailableSeats(); }

    void saveToFile() {
        if (!initialized) { printError("No seat data to save."); return; }
        ofstream file("seats.txt");
        if (!file) { printError("Could not open seats.txt for writing."); return; }
        file << totalSeats << "\n";
        for (int i = 1; i <= totalSeats; i++) {
            HashTable::SeatNode* h = hashTable.search(i);
            if (h != NULL) {
                file << h->seatNumber << " " << h->booked << " " << h->passengername << "\n";
            }
        }
        file.close();
        printSuccess("Seats saved to seats.txt");
        // Bonus: also save BST tree structure
        bst.saveTreeStructure();
    }

    void loadFromFile(Stack& logger) {
        ifstream file("seats.txt");
        if (!file) { printError("seats.txt not found."); return; }
        int count;
        file >> count;
        file.ignore();
        initializeSeats(count, logger);
        for (int i = 0; i < count; i++) {
            int seatNo;
            bool booked;
            string name;
            file >> seatNo >> booked;
            file.ignore();
            getline(file, name);
            if (booked) {
                HashTable::SeatNode* h = hashTable.search(seatNo);
                if (h) {
                    h->booked = true;
                    h->passengername = name;
                    bst.updatestatus(seatNo, true, name);
                }
            }
        }
        file.close();
        printSuccess("Seats loaded from seats.txt");
    }
};



//  Module 1: Train (AVL Tree node)
struct Train {
    int    id;
    string name;
    int    totalCars;
    CoachList coaches;

    Train() {
        id = 0;
        name = " ";
        totalCars = 0;
    }

    Train(int i, const string& n, int c) {
        id = i;
        name = n;
        totalCars = c;
    }
};


struct Node {
    Train  train;
    Node* left;
    Node* right;
    int    height;

    Node(const Train& t) {
        train = t;
        left = right = nullptr;
        height = 1;
    }
};


//  Module 1: Train Registry (AVL Tree)


class TrainRegistry {
private:
    Node* root;

    int height(Node* n) const {
        if (!n) return 0;
        else    return n->height;
    }

    void updateHeight(Node* n) const {
        if (n) {
            n->height = 1 + max(height(n->left), height(n->right));
        }
    }

    int balanceFactor(Node* n) const {
        if (n) return height(n->left) - height(n->right);
        else   return 0;
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* rebalance(Node* n) {
        updateHeight(n);
        int bf = balanceFactor(n);
        if (bf > 1) {
            if (balanceFactor(n->left) < 0)
                n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (bf < -1) {
            if (balanceFactor(n->right) > 0)
                n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    Node* insert(Node* n, const Train& t) {
        if (!n) return new Node(t);
        if (t.id < n->train.id)
            n->left = insert(n->left, t);
        else if (t.id > n->train.id)
            n->right = insert(n->right, t);
        else {
            printError("Train ID " + to_string(t.id) + " already exists in the registry.");
            return n;
        }
        return rebalance(n);
    }

    Node* minNode(Node* n) {
        while (n->left) n = n->left;
        return n;
    }

    Node* remove(Node* n, int id, bool& found) {
        if (!n) return nullptr;
        if (id < n->train.id) {
            n->left = remove(n->left, id, found);
        }
        else if (id > n->train.id) {
            n->right = remove(n->right, id, found);
        }
        else {
            found = true;
            if (!n->left || !n->right) {
                Node* child = n->left ? n->left : n->right;
                delete n;
                return child;
            }
            Node* successor = minNode(n->right);
            n->train = successor->train;
            bool dummy = false;
            n->right = remove(n->right, successor->train.id, dummy);
        }
        return rebalance(n);
    }

    Node* search(Node* n, int id) const {
        if (!n || n->train.id == id) return n;
        if (id < n->train.id) return search(n->left, id);
        else                  return search(n->right, id);
    }

    void inOrder(Node* n) const {
        if (!n) return;
        inOrder(n->left);
        cout << FG_CYAN "| " CLR_RESET;
        cout << FG_BYELLOW "[" << n->train.id << "]" CLR_RESET " ";
        cout << FG_BWHITE << n->train.name << CLR_RESET;
        int used = 2 + 3 + (int)to_string(n->train.id).size() + 1 + (int)n->train.name.size();
        for (int i = used; i < 43; i++) cout << " ";
        cout << FG_BCYAN "Cars: " CLR_RESET FG_BGREEN << n->train.totalCars << CLR_RESET;
        cout << "  " FG_CYAN "|" CLR_RESET "\n";
        inOrder(n->right);
    }

    void preOrder(Node* n) const {
        if (!n) return;
        cout << FG_CYAN "| " CLR_RESET;
        cout << FG_BYELLOW "[" << n->train.id << "]" CLR_RESET " ";
        cout << FG_BWHITE << n->train.name << CLR_RESET;
        int used = 2 + 3 + (int)to_string(n->train.id).size() + 1 + (int)n->train.name.size();
        for (int i = used; i < 43; i++) cout << " ";
        cout << FG_BCYAN "Cars: " CLR_RESET FG_BGREEN << n->train.totalCars << CLR_RESET;
        cout << "  " FG_CYAN "|" CLR_RESET "\n";
        preOrder(n->left);
        preOrder(n->right);
    }

    void postOrder(Node* n) const {
        if (!n) return;
        postOrder(n->left);
        postOrder(n->right);
        cout << FG_CYAN "| " CLR_RESET;
        cout << FG_BYELLOW "[" << n->train.id << "]" CLR_RESET " ";
        cout << FG_BWHITE << n->train.name << CLR_RESET;
        int used = 2 + 3 + (int)to_string(n->train.id).size() + 1 + (int)n->train.name.size();
        for (int i = used; i < 43; i++) cout << " ";
        cout << FG_BCYAN "Cars: " CLR_RESET FG_BGREEN << n->train.totalCars << CLR_RESET;
        cout << "  " FG_CYAN "|" CLR_RESET "\n";
        postOrder(n->right);
    }

    int count(Node* n) const {
        if (!n) return 0;
        else    return 1 + count(n->left) + count(n->right);
    }

    void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    void saveInOrder(Node* n, ofstream& file) {
        if (!n) return;
        saveInOrder(n->left, file);
        file << n->train.id << " " << n->train.totalCars << " " << n->train.name << "\n";
        Coach* curr = n->train.coaches.getHead();
        int coachCount = n->train.coaches.totalCoaches();
        file << coachCount << "\n";
        while (curr) {
            file << curr->id << " " << curr->name << "\n";
            curr = curr->next;
        }
        saveInOrder(n->right, file);
    }

    // Bonus: save AVL tree in pre-order to preserve hierarchy
    void savePreOrder(Node* n, ofstream& file) {
        if (!n) {
            file << "NULL\n";
            return;
        }
        file << n->train.id << " " << n->train.totalCars << " " << n->train.name << "\n";
        Coach* curr = n->train.coaches.getHead();
        file << n->train.coaches.totalCoaches() << "\n";
        while (curr) {
            file << curr->id << " " << curr->name << "\n";
            curr = curr->next;
        }
        savePreOrder(n->left, file);
        savePreOrder(n->right, file);
    }

public:

    TrainRegistry() {
        root = nullptr;
    }
    ~TrainRegistry() { destroy(root); }

    void registerTrain(int id, const string& name, int cars, Stack& logger, ActionStack& undoStack) {
        root = insert(root, Train(id, name, cars));
        printSuccess("Train registered: [" + to_string(id) + "] " + name);
        logger.push("Train [" + to_string(id) + "] " + name + " registered.");
        undoStack.push({ "REGISTER_TRAIN", to_string(id) + "|" + name + "|" + to_string(cars) });
    }

    void decommissionTrain(int id, Stack& logger, ActionStack& undoStack) {
        Node* found_node = getTrainNode(id);
        if (!found_node) {
            printError("Train " + to_string(id) + " not found in the registry.");
            return;
        }
        string savedName = found_node->train.name;
        int    savedCars = found_node->train.totalCars;
        bool   found = false;
        root = remove(root, id, found);
        if (found) {
            printSuccess("Train [" + to_string(id) + "] has been decommissioned.");
            logger.push("Train [" + to_string(id) + "] decommissioned.");
            undoStack.push({ "DECOMMISSION_TRAIN", to_string(id) + "|" + savedName + "|" + to_string(savedCars) });
        }
    }

    void findTrain(int id) const {
        Node* result = search(root, id);
        printSubTitle("Train Search");
        if (result) {
            printRow("ID", to_string(result->train.id));
            printRow("Name", result->train.name);
            printRow("Cars", to_string(result->train.totalCars));
        }
        else {
            printError("Train " + to_string(id) + " not found.");
        }
        printLine();
    }

    Node* getTrainNode(int id) const {
        return search(root, id);
    }

    void patrolByID() const {
        printSubTitle("Fleet Patrol - Sorted by ID");
        inOrder(root);
        printLine();
    }

    void patrolTopDown() const {
        printSubTitle("Fleet Patrol - Top Down (Pre-Order)");
        preOrder(root);
        printLine();
    }

    void patrolBottomUp() const {
        printSubTitle("Fleet Patrol - Bottom Up (Post-Order)");
        postOrder(root);
        printLine();
    }

    int totalTrains()    const { return count(root); }
    int registryHeight() const { return height(root); }

    void saveToFile() {
        ofstream file("trains.txt");
        if (!file) { printError("Could not open trains.txt for writing."); return; }
        file << count(root) << "\n";
        saveInOrder(root, file);
        file.close();
        printSuccess("Trains saved to trains.txt");

        // Bonus: save AVL tree structure preserving hierarchy
        ofstream treeFile("trains_tree.txt");
        if (treeFile) {
            treeFile << "# AVL Tree - PreOrder Dump (structure-preserving)\n";
            treeFile << "# Format: id totalCars name | coachCount | coaches\n";
            treeFile << "# NULL = missing child (preserves shape on reload)\n";
            savePreOrder(root, treeFile);
            treeFile.close();
            printSuccess("AVL tree structure saved to trains_tree.txt");
        }
    }

    void loadFromFile(Stack& logger, ActionStack& undoStack) {
        ifstream file("trains.txt");
        if (!file) { printError("trains.txt not found."); return; }
        int total;
        file >> total;
        file.ignore();
        for (int i = 0; i < total; i++) {
            int id, cars;
            string name;
            file >> id >> cars;
            file.ignore();
            getline(file, name);
            registerTrain(id, name, cars, logger, undoStack);

            int coachCount;
            file >> coachCount;
            file.ignore();
            Node* trainNode = getTrainNode(id);
            if (trainNode) {
                for (int j = 0; j < coachCount; j++) {
                    int cid;
                    string cname;
                    file >> cid;
                    file.ignore();
                    getline(file, cname);
                    trainNode->train.coaches.addAtEnd(cid, cname, logger, undoStack);
                }
            }
        }
        file.close();
        printSuccess("Trains loaded from trains.txt");
    }
};

//  Bonus: Undo / Redo Engine

string parseField(string data, int n) {
    int start = 0;
    for (int i = 0; i < n; i++) {
        start = data.find('|', start) + 1;
    }
    int end = data.find('|', start);
    return data.substr(start, end == (int)string::npos ? string::npos : end - start);
}

void performUndo(ActionStack& undoStack, ActionStack& redoStack,
    TrainRegistry& registry, RailNetwork& network,
    SeatingChart& seats, Stack& logger) {

    if (undoStack.isEmpty()) {
        printError("Nothing to undo.");
        return;
    }

    Action a = undoStack.pop();
    printUndo("Undoing: " + a.type);

    if (a.type == "REGISTER_TRAIN") {
        int id = stoi(parseField(a.data, 0));
        logger.push("UNDO: Train [" + to_string(id) + "] registration reversed.");
        redoStack.push(a);
    }
    else if (a.type == "DECOMMISSION_TRAIN") {
        int    id = stoi(parseField(a.data, 0));
        string name = parseField(a.data, 1);
        int    cars = stoi(parseField(a.data, 2));
        ActionStack dummy;
        registry.registerTrain(id, name, cars, logger, dummy);
        logger.push("UNDO: Train [" + to_string(id) + "] decommission reversed.");
        redoStack.push(a);
    }
    else if (a.type == "BOOK_SEAT") {
        int seatNo = stoi(parseField(a.data, 0));
        seats.directCancel(seatNo);
        logger.push("UNDO: Seat [" + to_string(seatNo) + "] booking reversed.");
        redoStack.push(a);
    }
    else if (a.type == "CANCEL_SEAT") {
        int    seatNo = stoi(parseField(a.data, 0));
        string name = parseField(a.data, 1);
        seats.directBook(seatNo, name);
        logger.push("UNDO: Seat [" + to_string(seatNo) + "] cancellation reversed.");
        redoStack.push(a);
    }
    else if (a.type == "ADD_STATION") {
        int id = stoi(parseField(a.data, 0));
        ActionStack dummy;
        network.removeStation(id, logger, dummy);
        logger.push("UNDO: Station [" + to_string(id) + "] addition reversed.");
        redoStack.push(a);
    }
    else if (a.type == "REMOVE_STATION") {
        int id = stoi(parseField(a.data, 0));
        network.reactivateStation(id);
        logger.push("UNDO: Station [" + to_string(id) + "] removal reversed.");
        redoStack.push(a);
    }
    else if (a.type == "ADD_TRACK") {
        int from = stoi(parseField(a.data, 0));
        int to = stoi(parseField(a.data, 1));
        network.setTrack(from, to, 0);
        logger.push("UNDO: Track [" + to_string(from) + "]-[" + to_string(to) + "] addition reversed.");
        redoStack.push(a);
    }
    else if (a.type == "REMOVE_TRACK") {
        int from = stoi(parseField(a.data, 0));
        int to = stoi(parseField(a.data, 1));
        int weight = stoi(parseField(a.data, 2));
        network.setTrack(from, to, weight);
        logger.push("UNDO: Track [" + to_string(from) + "]-[" + to_string(to) + "] removal reversed.");
        redoStack.push(a);
    }
    else {
        printError("Undo not supported for: " + a.type);
    }
}

void performRedo(ActionStack& undoStack, ActionStack& redoStack,
    TrainRegistry& registry, RailNetwork& network,
    SeatingChart& seats, Stack& logger) {

    if (redoStack.isEmpty()) {
        printError("Nothing to redo.");
        return;
    }

    Action a = redoStack.pop();
    printRedo("Redoing: " + a.type);

    if (a.type == "REGISTER_TRAIN") {
        int    id = stoi(parseField(a.data, 0));
        string name = parseField(a.data, 1);
        int    cars = stoi(parseField(a.data, 2));
        registry.registerTrain(id, name, cars, logger, undoStack);
        logger.push("REDO: Train [" + to_string(id) + "] re-registered.");
    }
    else if (a.type == "DECOMMISSION_TRAIN") {
        int id = stoi(parseField(a.data, 0));
        registry.decommissionTrain(id, logger, undoStack);
        logger.push("REDO: Train [" + to_string(id) + "] re-decommissioned.");
    }
    else if (a.type == "BOOK_SEAT") {
        int    seatNo = stoi(parseField(a.data, 0));
        string name = parseField(a.data, 1);
        seats.directBook(seatNo, name);
        logger.push("REDO: Seat [" + to_string(seatNo) + "] re-booked.");
        undoStack.push(a);
    }
    else if (a.type == "CANCEL_SEAT") {
        int seatNo = stoi(parseField(a.data, 0));
        seats.directCancel(seatNo);
        logger.push("REDO: Seat [" + to_string(seatNo) + "] re-cancelled.");
        undoStack.push(a);
    }
    else if (a.type == "ADD_STATION") {
        int    id = stoi(parseField(a.data, 0));
        string name = parseField(a.data, 1);
        network.reactivateStation(id);
        logger.push("REDO: Station [" + to_string(id) + "] re-added.");
        undoStack.push(a);
    }
    else if (a.type == "REMOVE_STATION") {
        int id = stoi(parseField(a.data, 0));
        ActionStack dummy;
        network.removeStation(id, logger, dummy);
        logger.push("REDO: Station [" + to_string(id) + "] re-removed.");
        undoStack.push(a);
    }
    else if (a.type == "ADD_TRACK") {
        int from = stoi(parseField(a.data, 0));
        int to = stoi(parseField(a.data, 1));
        int weight = stoi(parseField(a.data, 2));
        network.setTrack(from, to, weight);
        logger.push("REDO: Track [" + to_string(from) + "]-[" + to_string(to) + "] re-added.");
        undoStack.push(a);
    }
    else if (a.type == "REMOVE_TRACK") {
        int from = stoi(parseField(a.data, 0));
        int to = stoi(parseField(a.data, 1));
        network.setTrack(from, to, 0);
        logger.push("REDO: Track [" + to_string(from) + "]-[" + to_string(to) + "] re-removed.");
        undoStack.push(a);
    }
    else {
        printError("Redo not supported for: " + a.type);
    }
}

//  Global Save / Load

void saveAll(TrainRegistry& registry, RailNetwork& network, SeatingChart& seats, Stack& logger) {
    registry.saveToFile();
    network.saveToFile();
    seats.saveToFile();
    logger.saveToFile();
    printSuccess("All modules saved.");
}

void loadAll(TrainRegistry& registry, RailNetwork& network, SeatingChart& seats, Stack& logger, ActionStack& undoStack) {
    registry.loadFromFile(logger, undoStack);
    network.loadFromFile();
    seats.loadFromFile(logger);
    logger.loadFromFile();
    printSuccess("All modules loaded.");
}


void menuTrainRegistry(TrainRegistry& registry, Stack& logger, ActionStack& undoStack) {
    int choice;
    do {
        printTitle("MODULE 1  |  Train Registry");
        printRow("1. Register Train");
        printRow("2. Decommission Train");
        printRow("3. Find Train by ID");
        printRow("4. Patrol by ID (In-Order)");
        printRow("5. Patrol Top-Down (Pre-Order)");
        printRow("6. Patrol Bottom-Up (Post-Order)");
        printRow("7. Registry Stats");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) {
            int id, cars;
            string name;
            cout << FG_BYELLOW "  Train ID: " CLR_RESET; cin >> id; cin.ignore();
            cout << FG_BYELLOW "  Train Name: " CLR_RESET; getline(cin, name);
            cout << FG_BYELLOW "  Total Cars: " CLR_RESET; cin >> cars; cin.ignore();
            registry.registerTrain(id, name, cars, logger, undoStack);
        }
        else if (choice == 2) {
            int id;
            cout << FG_BYELLOW "  Train ID to decommission: " CLR_RESET; cin >> id; cin.ignore();
            registry.decommissionTrain(id, logger, undoStack);
        }
        else if (choice == 3) {
            int id;
            cout << FG_BYELLOW "  Train ID to search: " CLR_RESET; cin >> id; cin.ignore();
            registry.findTrain(id);
        }
        else if (choice == 4) { registry.patrolByID(); }
        else if (choice == 5) { registry.patrolTopDown(); }
        else if (choice == 6) { registry.patrolBottomUp(); }
        else if (choice == 7) {
            printSubTitle("Registry Stats");
            printRow("Total Trains", to_string(registry.totalTrains()));
            printRow("Tree Height", to_string(registry.registryHeight()));
            printLine();
        }
        cout << "\n";
    } while (choice != 0);
}

void menuCoachManagement(TrainRegistry& registry, Stack& logger, ActionStack& undoStack) {
    int choice;
    do {
        printTitle("MODULE 2  |  Coach Management");
        printRow("1. Add Coach at Front");
        printRow("2. Add Coach at End");
        printRow("3. Add Coach After ID");
        printRow("4. Remove Coach");
        printRow("5. Traverse Forward");
        printRow("6. Traverse Backward");
        printRow("7. Reverse Train");
        printRow("8. Coach Stats");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        int trainId;
        cout << FG_BYELLOW "  Train ID: " CLR_RESET; cin >> trainId; cin.ignore();
        Node* trainNode = registry.getTrainNode(trainId);
        if (!trainNode && choice != 0) {
            printError("Train not found.");
            cout << "\n";
            continue;
        }

        if (choice == 1) {
            int cid; string cname;
            cout << FG_BYELLOW "  Coach ID: " CLR_RESET; cin >> cid; cin.ignore();
            cout << FG_BYELLOW "  Coach Name: " CLR_RESET; getline(cin, cname);
            trainNode->train.coaches.addAtFront(cid, cname, logger, undoStack);
        }
        else if (choice == 2) {
            int cid; string cname;
            cout << FG_BYELLOW "  Coach ID: " CLR_RESET; cin >> cid; cin.ignore();
            cout << FG_BYELLOW "  Coach Name: " CLR_RESET; getline(cin, cname);
            trainNode->train.coaches.addAtEnd(cid, cname, logger, undoStack);
        }
        else if (choice == 3) {
            int afterId, cid; string cname;
            cout << FG_BYELLOW "  Insert After Coach ID: " CLR_RESET; cin >> afterId; cin.ignore();
            cout << FG_BYELLOW "  New Coach ID: " CLR_RESET; cin >> cid; cin.ignore();
            cout << FG_BYELLOW "  New Coach Name: " CLR_RESET; getline(cin, cname);
            trainNode->train.coaches.addAfter(afterId, cid, cname, logger, undoStack);
        }
        else if (choice == 4) {
            int cid;
            cout << FG_BYELLOW "  Coach ID to remove: " CLR_RESET; cin >> cid; cin.ignore();
            trainNode->train.coaches.removeCoach(cid, logger, undoStack);
        }
        else if (choice == 5) { trainNode->train.coaches.traverseForward(); }
        else if (choice == 6) { trainNode->train.coaches.traverseBackward(); }
        else if (choice == 7) { trainNode->train.coaches.reverse(logger, undoStack); }
        else if (choice == 8) {
            printSubTitle("Coach Stats");
            printRow("Total Coaches", to_string(trainNode->train.coaches.totalCoaches()));
            printLine();
        }
        cout << "\n";
    } while (choice != 0);
}

void menuRailNetwork(RailNetwork& network, Stack& logger, ActionStack& undoStack) {
    int choice;
    do {
        printTitle("MODULE 3  |  Rail Network");
        printRow("1. Add Station");
        printRow("2. Remove Station");
        printRow("3. Add Track");
        printRow("4. Remove Track");
        printRow("5. Display Network");
        printRow("6. Find Shortest Path");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) {
            int id; string name;
            cout << FG_BYELLOW "  Station ID: " CLR_RESET; cin >> id; cin.ignore();
            cout << FG_BYELLOW "  Station Name: " CLR_RESET; getline(cin, name);
            network.addStation(id, name, logger, undoStack);
        }
        else if (choice == 2) {
            int id;
            cout << FG_BYELLOW "  Station ID to remove: " CLR_RESET; cin >> id; cin.ignore();
            network.removeStation(id, logger, undoStack);
        }
        else if (choice == 3) {
            int from, to, weight;
            cout << FG_BYELLOW "  From Station ID: " CLR_RESET; cin >> from; cin.ignore();
            cout << FG_BYELLOW "  To Station ID: " CLR_RESET; cin >> to; cin.ignore();
            cout << FG_BYELLOW "  Distance (km): " CLR_RESET; cin >> weight; cin.ignore();
            network.addTrack(from, to, weight, logger, undoStack);
        }
        else if (choice == 4) {
            int from, to;
            cout << FG_BYELLOW "  From Station ID: " CLR_RESET; cin >> from; cin.ignore();
            cout << FG_BYELLOW "  To Station ID: " CLR_RESET; cin >> to; cin.ignore();
            network.removeTrack(from, to, logger, undoStack);
        }
        else if (choice == 5) { network.displayNetwork(); }
        else if (choice == 6) {
            int from, to;
            cout << FG_BYELLOW "  From Station ID: " CLR_RESET; cin >> from; cin.ignore();
            cout << FG_BYELLOW "  To Station ID: " CLR_RESET; cin >> to; cin.ignore();
            network.shortestPath(from, to);
        }
        cout << "\n";
    } while (choice != 0);
}

void menuSeatBooking(SeatingChart& seats, Stack& logger, ActionStack& undoStack) {
    int choice;
    do {
        printTitle("MODULE 4  |  Seat Booking");
        printRow("1. Initialize Seats");
        printRow("2. Book a Seat");
        printRow("3. Cancel a Seat");
        printRow("4. Check Seat Status");
        printRow("5. Display Booked Seats");
        printRow("6. Display Available Seats");
        printRow("7. Display All Seats Sorted");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) {
            int total;
            cout << FG_BYELLOW "  Number of seats: " CLR_RESET; cin >> total; cin.ignore();
            seats.initializeSeats(total, logger);
        }
        else if (choice == 2) {
            int seatNo; string name;
            cout << FG_BYELLOW "  Seat Number: " CLR_RESET; cin >> seatNo; cin.ignore();
            cout << FG_BYELLOW "  Passenger Name: " CLR_RESET; getline(cin, name);
            seats.bookSeat(seatNo, name, logger, undoStack);
        }
        else if (choice == 3) {
            int seatNo;
            cout << FG_BYELLOW "  Seat Number to cancel: " CLR_RESET; cin >> seatNo; cin.ignore();
            seats.cancelSeat(seatNo, logger, undoStack);
        }
        else if (choice == 4) {
            int seatNo;
            cout << FG_BYELLOW "  Seat Number: " CLR_RESET; cin >> seatNo; cin.ignore();
            seats.checkSeat(seatNo);
        }
        else if (choice == 5) { seats.displayBookedSeats(); }
        else if (choice == 6) { seats.displayAvailableSeats(); }
        else if (choice == 7) { seats.displaySorted(); }
        cout << "\n";
    } while (choice != 0);
}

void menuOperationLog(Stack& logger) {
    int choice;
    do {
        printTitle("MODULE 5  |  Operation Log");
        printRow("1. Display Full Log");
        printRow("2. Peek Most Recent Entry");
        printRow("3. Pop Most Recent Entry");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) { logger.display(); }
        else if (choice == 2) {
            printSubTitle("Most Recent Entry");
            printRow(logger.peek());
            printLine();
        }
        else if (choice == 3) {
            printSubTitle("Pop Entry");
            printRow("Removed", logger.pop());
            printRow("Now Recent", logger.peek());
            printLine();
        }
        cout << "\n";
    } while (choice != 0);
}

void menuUndoRedo(ActionStack& undoStack, ActionStack& redoStack,
    TrainRegistry& registry, RailNetwork& network,
    SeatingChart& seats, Stack& logger) {
    int choice;
    do {
        printTitle("BONUS  |  Undo / Redo");
        printRow("1. Undo Last Action");
        printRow("2. Redo Last Undone Action");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) { performUndo(undoStack, redoStack, registry, network, seats, logger); }
        else if (choice == 2) { performRedo(undoStack, redoStack, registry, network, seats, logger); }
        cout << "\n";
    } while (choice != 0);
}

void menuSaveLoad(TrainRegistry& registry, RailNetwork& network,
    SeatingChart& seats, Stack& logger, ActionStack& undoStack) {
    int choice;
    do {
        printTitle("THE IRON NEXUS  |  Save & Load");
        printRow("1. Save All Modules");
        printRow("2. Load All Modules");
        printRow("0. Back to Main Menu");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) { saveAll(registry, network, seats, logger); }
        else if (choice == 2) { loadAll(registry, network, seats, logger, undoStack); }
        cout << "\n";
    } while (choice != 0);
}

int main() {

    Stack       logger;
    ActionStack undoStack;
    ActionStack redoStack;

    TrainRegistry registry;
    RailNetwork   network;
    SeatingChart  seats;

    int choice;
    do {
        printTitle("THE IRON NEXUS  |  Main Menu");
        printRow("1. Train Registry");
        printRow("2. Coach Management");
        printRow("3. Rail Network");
        printRow("4. Seat Booking");
        printRow("5. Operation Log");
        printRow("6. Undo / Redo");
        printRow("7. Save & Load");
        printRow("0. Exit");
        printLine();
        cout << FG_BYELLOW "  Enter choice: " CLR_RESET;
        cin >> choice;
        cin.ignore();
        cout << "\n";

        if (choice == 1) { menuTrainRegistry(registry, logger, undoStack); }
        else if (choice == 2) { menuCoachManagement(registry, logger, undoStack); }
        else if (choice == 3) { menuRailNetwork(network, logger, undoStack); }
        else if (choice == 4) { menuSeatBooking(seats, logger, undoStack); }
        else if (choice == 5) { menuOperationLog(logger); }
        else if (choice == 6) { menuUndoRedo(undoStack, redoStack, registry, network, seats, logger); }
        else if (choice == 7) { menuSaveLoad(registry, network, seats, logger, undoStack); }
        else if (choice == 0) {
            printTitle("Session Complete");
            printLine();
        }
    } while (choice != 0);

    return 0;
}