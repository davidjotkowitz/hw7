#ifndef GRADES_H
#define GRADES_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "grades.h"
#include "linked-list.h"


/*Each student holds an Id, a name, a list of the courses he or she takes and
the total amount of points he or she has. The last is used for calculating his 
or hers average.*/
struct student{
	int id;
	char *name;
	struct list *courses;
	float points;
};

/*Every course has its name and the points the student recieved in it. */
struct course_single{
	char *name;
	int grade;
};

/*These are the user functions needed to operate linked-list.h. */
int student_clone(void *element, void **output);
int course_clone(void *element, void **output);
void student_destroy(void *element);
void course_destroy(void *element);


/*From here all functions have been explained in the grades.h file.*/
int student_clone(void *element, void **output){

	struct student *student;
	struct student *new_student;	
	student=(struct student*)element;
	new_student=(struct student*)malloc(sizeof(*new_student));
	if (!new_student) {
		/* Out of memory... */
		free(new_student);
		return -1;
	}
//Here we copy all the data, and initiate a list of courses for the clone.
	new_student->id=student->id;
	new_student->name=strdup(student->name);
	new_student->points=student->points;
	new_student->courses=list_init(course_clone,course_destroy);
	
	struct node* it= list_begin(student->courses);	
	struct course_single *temp_c;
	struct course_single *new_course; 

//Here we clone each course individually into the clone's list of courses.	
	while (it){                  		
		temp_c=(struct course_single*)list_get(it);		
		new_course=(struct course_single*)malloc(sizeof(*new_course));
		if (!new_course){
			free(new_course);
			return -1;
		}
  
		new_course->name=strdup(temp_c->name);   
		new_course->grade=temp_c->grade;			
		int check= list_push_back(new_student->courses,new_course);

//Because list_push_back only takes a clone, we must maually free all data 
//allocated for the new_student
		free(new_course->name);
		course_destroy(new_course); 
		if (check){
			list_destroy(new_student->courses);
			return -1;
		}
		it= list_next(it);
	}	
	*output=new_student;	
	return 0;
}

int course_clone(void *element, void **output){

	struct course_single *course;
	struct course_single *out;
	course=(struct course_single*)element;
	out=(struct course_single*)malloc(sizeof(*out));
	if (!out){
		free(out);
		return -1;
	}	
//here we clone the course's data into the clone.
	out->name=strdup(course->name);
	out->grade=course->grade;
	*output=out;         
	return 0;

}


void student_destroy(void *element){ 

	if(!element){
		return;
	}
	struct student *student;
	student=(struct student*)element;

//We must destroy the list that hold extra allocated data we cannot reach
//from the student. Also we must free the space allocated for the student's 
//name.
	list_destroy(student->courses); 
	free(student->name);
	free(student);
	
} 

void course_destroy(void *element){

	if(!element){
		return;
	}
	struct course_single *course=(struct course_single*)element;
	free(course->name);
	free(course);
}


//All we need to hold in "grades is a list of students
struct grades{
	struct list *students;
};



struct grades* grades_init(){

	struct grades *grades;
	grades=(struct grades*)malloc(sizeof(*grades));
//Here we Initialize an empty list of students.
	grades->students= list_init(student_clone, student_destroy); 
	return grades;

};

void grades_destroy(struct grades *grades){

	list_destroy(grades->students);
	free(grades);	
};



int grades_add_student(struct grades *grades, const char *name, int id){

	struct student *new_student;	
	struct node *it_students;
	it_students= list_begin(grades->students); 
	struct student *temp;

//Here we search if a student with the same id already exists in the list.
	while (it_students){		
		temp=(struct student*)list_get(it_students);   
		if((temp->id)==id){ 
			break;
		}
		it_students=list_next(it_students);
	}

	if(it_students){
		return -1;
	}

//Here we add the nececary data to the new student, to clone into the list.
	new_student=(struct student*)malloc(sizeof(*new_student));	
	new_student->id=id;
	new_student->name=strdup(name);
	new_student->points=0;
	new_student->courses=list_init(course_clone,course_destroy);	
	int check=list_push_back(grades->students, new_student);	
	student_destroy(new_student);	
	
	if(check){
		return -1;
	}
	else {
		return 0;
	}
};


int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade){

	struct node *it_students;
	it_students= list_begin(grades->students); 	
	struct student* temp;

//Here we search for the student.
	while (it_students){		
		temp=(struct student*)list_get(it_students);
		if((temp->id)==id){
			break;
		}
		it_students=list_next(it_students);
	}

	
	if (it_students){		
		struct node *it_courses= list_begin(temp->courses); 		
		struct course_single* temp_c;		

//Here we make sure the course doesn't already exist.
		while (it_courses){			
			temp_c=(struct course_single*)list_get(it_courses);
			if(!strcmp(temp_c->name,name)){
				break;
			}
			it_courses=list_next(it_courses);		
		}

		if(!it_courses && grade<101 && grade>=0){

//Here we add the data, and clone it into the list.		
		struct course_single *new_course;  //duplicate the element?
		new_course=(struct course_single*)malloc(sizeof(*new_course));				
		new_course->name=strdup(name); 
		new_course->grade=(float)grade;			
		int chk= list_push_back(temp->courses, new_course);	 //problem!!!!!!!! doesnt return 0, not working		
		free(new_course->name);
		free(new_course);
		if(chk){
			return -1;
		}
//It's important that we add the points to the total, so we can have the 
//average ready.
		temp->points += grade;		
		return 0;
		}
	}
	return -1;
};


float grades_calc_avg(struct grades *grades, int id, char **out){ 

	struct node *it_students= list_begin(grades->students); 	
	struct student* temp;
	while (it_students){
		
		temp=(struct student*)list_get(it_students);
		if((temp->id)==id){
			break;
		}
		it_students=list_next(it_students);
	}
	if (it_students){		
		*out=strdup(temp->name); //or maybe a &?

//Here we find the denomonater of the average by using a "linked-list" fuction.
		int num_courses=list_size(temp->courses);
		if (num_courses==0){
			return 0;
		}
		else return ((temp->points)/num_courses);	
	}
	return -1;
};


int grades_print_student(struct grades *grades, int id){

	struct node *it_students= list_begin(grades->students); 	
	struct student* temp;

//Here we find the student.
	while (it_students){		
		temp=(struct student*)list_get(it_students);
		if((temp->id)==id){
			break;
		}
		it_students=list_next(it_students);
	}
	
	if (it_students){		

//Here are all the printing functions.
		printf("%s %d:",temp->name,temp->id);		
		struct node *it_courses= list_begin(temp->courses);		
		struct course_single* temp_c;
		while (it_courses){			
			temp_c=(struct course_single*)list_get(it_courses);
			if (it_courses!=list_begin(temp->courses)){
				printf(", %s %d",temp_c->name,temp_c->grade);
			} else {
				printf(" %s %d",temp_c->name,temp_c->grade);
			}			
			it_courses=list_next(it_courses);
		}		
		printf("\n");
		return 0;	
	}
	return -1;
};


int grades_print_all(struct grades *grades){    //didnt account for an error

	struct node *it_students= list_begin(grades->students); 
	struct student* temp;		
	temp=(struct student*)list_get(it_students);		

//We go over every student and print him or her and all of his or her courses.
	while (it_students){		
		temp=(struct student*)list_get(it_students);	
		printf("%s %d:",temp->name,temp->id);		
		struct node *it_courses= list_begin(temp->courses);		
		struct course_single* temp_c;
		while (it_courses){			
			temp_c=(struct course_single*)list_get(it_courses);
			if (it_courses!=list_begin(temp->courses)){
				printf(", %s %d",temp_c->name,temp_c->grade);
			}
			else {
				printf(" %s %d",temp_c->name,temp_c->grade);
			}			
			it_courses=list_next(it_courses);
		}
		printf("\n");
		it_students=list_next(it_students);		
	}
	return 0;
};

#endif


