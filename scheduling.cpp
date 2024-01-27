#include "avlbst.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>

using namespace std;

struct Student{
    set<int> studentClasses;
};

bool assignTimes(set<int>& exams, Student** students, AVLTree<int, int>& examTimes, int numTimes, vector<int>& classes, int idx, int numStudents, int numExams);
bool checkStudentSchedules(Student** students, set<int>& conflictingExams, int newClass, int numStudents);

int main(int agrc, char* argv[]){
    //Get the number of students, exams and timeslots
    ifstream ifile(argv[1]);
    int numStudents, numExams, numTimes;
    ifile >> numExams >> numStudents >> numTimes;
    ifile.ignore();

    Student** students = new Student*[numStudents];
    set<int> exams;
    vector<int> tests;

    for (int i = 0; i < numStudents; i++){
        string courses;
        ifile >> courses;   //Temporarily contains the name of the student
        students[i] = new Student;
        getline(ifile, courses);    //Get all the exams this student is taking
        stringstream ss(courses);
        
        //Store every exam being administered and for every student, store the exams the are taking
        int course;
        while (ss >> course){
            exams.insert(course);
            students[i]->studentClasses.insert(course);
            tests.push_back(course);
        }
    }

    AVLTree<int, int> examTimes;
    bool success = assignTimes(exams, students, examTimes, numTimes, tests, 0, numStudents, numExams);

    //Print results
    if (success == true){
        for (BinarySearchTree<int, int>::iterator it = examTimes.begin(); it != examTimes.end(); ++it){
            cout << it->first << " " << it->second << endl;
        }
    }
    else cout << "No valid solution." << endl;

    return 0;
}

bool assignTimes(set<int>& exams, Student** students, AVLTree<int, int>& examTimes, int numTimes, vector<int>& classes, int idx, int numStudents, int numExams){
    //Base Case: all exams have been scheduled
    int examsAssigned = 0;
    for (BinarySearchTree<int, int>::iterator it = examTimes.begin(); it != examTimes.end(); ++it){
        examsAssigned++;
    }
    if (examsAssigned == numExams) return true;

    //Check if the current exam has already been scheduled
    if (examTimes.find(classes[idx]) != examTimes.end()){
        assignTimes(exams, students, examTimes, numTimes, classes, idx + 1, numStudents, numExams);
    }

    //Try scheduling the exam at every time
    for (int time = 1; time <= numTimes; time++){
        bool conflict = false;
        set<int> conflictingExams = {};

        //Get all the exams that are already scheduled at this time
        for (BinarySearchTree<int, int>::iterator it = examTimes.begin(); it != examTimes.end(); ++it){
            if (it->second == time){
                conflict = true;
                conflictingExams.insert(it->first);
            }
        }

        /*If no other exam is scheduled at this time, or no students are taking both this exam and the other exams
        scheduled at this time, schedule the exam*/
        if (conflict == false || checkStudentSchedules(students, conflictingExams, classes[idx], numStudents)){
            examTimes.insert(pair<int, int>(classes[idx], time));
            if (assignTimes(exams, students, examTimes, numTimes, classes, idx + 1, numStudents, numExams)) return true;
        }
        
        examTimes.remove(classes[idx]); //Backtrack: don't schedule the exam at this time, then try adding it at another time
    }
    return false;
}


bool checkStudentSchedules(Student** students, set<int>& conflictingExams, int newClass, int numStudents){
    //Check every exam that conflicts with the new exam
    for (auto it = conflictingExams.begin(); it != conflictingExams.end(); ++it){
        for (int i = 0; i < numStudents; i++){
            int oldClass = *it;
            bool taking_new_class = false;
            bool taking_old_class = false;
            bool conflict = false;

            //Check if any student is taking both exams, and if so return false
            for (auto it1 = students[i]->studentClasses.begin(); it1 != students[i]->studentClasses.end(); ++it1){
                if (*it1 == newClass) taking_new_class = true;
                if (*it1 == oldClass) taking_old_class = true;
            
                if (taking_new_class && taking_old_class){
                    conflict = true;
                    break;
                }
            }
            if (conflict) return false; 
        }
    }
    return true;   
}





