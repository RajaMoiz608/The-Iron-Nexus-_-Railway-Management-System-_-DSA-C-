#pragma once
#define CLASSES_H

#include <iostream>
#include <string>
#include <fstream>
#include "ColoredTerminal.h"

using namespace std;


// Stack

class Stack {
private:
    string arr[500];
    int top;

public:
    Stack() {
        top = -1;
    }

    bool isFull() {
        if (top >= 499) {
            return true;
        }
        else {
            return false;
        }
    }

    bool isEmpty() {
        if (top < 0) {
            return true;
        }
        else {
            return false;
        }
    }

    void push(string logEntry) {
        if (isFull()) {
            printError("Log is full. Cannot record more entries.");
        }
        else {
            top++;
            arr[top] = logEntry;
        }
    }

    string pop() {
        if (isEmpty()) {
            printError("Log is empty.");
            return "";
        }
        string val = arr[top];
        top--;
        return val;
    }

    string peek() {
        if (isEmpty()) {
            printError("Log is empty.");
            return "";
        }
        return arr[top];
    }

    void display() {
        printSubTitle("Operation Log (Most Recent First)");
        if (isEmpty()) {
            printRow("No logs recorded.");
            printLine();
            return;
        }
        for (int i = top; i >= 0; i--) {
            // alternate coloring for readability
            if ((top - i) % 2 == 0) {
                cout << FG_CYAN "| " CLR_RESET FG_BWHITE << arr[i] << CLR_RESET;
            }
            else {
                cout << FG_CYAN "| " CLR_RESET FG_WHITE << arr[i] << CLR_RESET;
            }
            int used = 2 + (int)arr[i].size();
            for (int j = used; j < 51; j++) cout << " ";
            cout << FG_CYAN "|" CLR_RESET "\n";
        }
        printLine();
    }

    void saveToFile() {
        ofstream file("log.txt");
        if (!file) {
            printError("Could not open log.txt for writing.");
            return;
        }
        file << top + 1 << "\n";
        for (int i = 0; i <= top; i++) {
            file << arr[i] << "\n";
        }
        file.close();
        printSuccess("Log saved to log.txt");
    }

    void loadFromFile() {
        ifstream file("log.txt");
        if (!file) {
            printError("log.txt not found.");
            return;
        }
        int count;
        file >> count;
        file.ignore();
        top = -1;
        for (int i = 0; i < count; i++) {
            string entry;
            getline(file, entry);
            push(entry);
        }
        file.close();
        printSuccess("Log loaded from log.txt");
    }
};





//  Bonus: Undo/Redo Action
struct Action {
    string type;
    string data;   // serialized parameters separated by |
};


//  Bonus: Undo/Redo Stack (separate from log)


class ActionStack {
private:
    Action arr[500];
    int top;

public:
    ActionStack() {
        top = -1;
    }

    bool isEmpty() {
        return top < 0;
    }

    bool isFull() {
        return top >= 499;
    }

    void push(Action a) {
        if (!isFull()) {
            top++;
            arr[top] = a;
        }
    }

    Action pop() {
        Action a;
        if (!isEmpty()) {
            a = arr[top];
            top--;
        }
        return a;
    }

    Action peek() {
        return arr[top];
    }

    void clear() {
        top = -1;
    }
};



// Priority Queue (Min-Heap) for Dijkstra Pathfinding

struct PQNode {
    int vertex;
    int distance;
};

class PriorityQueue {
private:
    PQNode heap[200];
    int size;

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].distance < heap[parent].distance) {

                PQNode temp = heap[index];
                heap[index] = heap[parent];
                heap[parent] = temp;
                index = parent;
            }
            else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        while (2 * index + 1 < size) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int smallest = leftChild;

            if (rightChild < size && heap[rightChild].distance < heap[leftChild].distance) {
                smallest = rightChild;
            }

            if (heap[index].distance > heap[smallest].distance) {

                PQNode temp = heap[index];
                heap[index] = heap[smallest];
                heap[smallest] = temp;
                index = smallest;
            }
            else {
                break;
            }
        }
    }

public:
    PriorityQueue() {
        size = 0;
    }

    bool isEmpty() {
        return size == 0;
    }

    void push(int vertex, int distance) {
        if (size >= 200) {
            printError("Priority Queue Overflow.");
            return;
        }
        heap[size].vertex = vertex;
        heap[size].distance = distance;
        heapifyUp(size);
        size++;
    }

    PQNode pop() {
        if (isEmpty()) {
            return PQNode{ -1, -1 };
        }
        PQNode topNode = heap[0];
        size--;
        heap[0] = heap[size];
        heapifyDown(0);
        return topNode;
    }
};