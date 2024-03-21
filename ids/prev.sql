drop table if exists users cascade;
drop table if exists student cascade;
drop table if exists teacher cascade;
drop table if exists subject cascade;
drop table if exists gradedActivity cascade;
drop table if exists enrollement cascade;
drop table if exists grade cascade;
drop table if exists teachingSubjects cascade;

drop type if exists ROLE cascade;
drop type if exists STUDY_TYPE cascade;
drop type if exists TITLE cascade;
drop type if exists LANGUAGE cascade;
drop type if exists POSITION cascade;

create type ROLE as enum ('student', 'teacher', 'admin');
create type STUDY_TYPE as enum ('daily', 'remote');
create type TITLE as enum ('N/A','Bc.', 'Phd.');
create type LANGUAGE as enum ('Czech','English');
create type POSITION as enum ('Helper','Teacher','Garant');

create table users (
	UserId int NOT NULL AUTO_INCREMENT,
	name varchar(255),
	surname varchar(255),
	email varchar(255),
    role ROLE default 'student',
    
    PRIMARY KEY (UserId)
);

create table student (
	StudentID int NOT NULL AUTO_INCREMENT,
    YearEnrolled date NOT NULL,
    Role STUDYTYPE default 'daily',
    CReditsGained int  default 0,
    CreditsEnrolled int default 0,
    UserID int reference users(UserId),
    PRIMARY KEY (StudentID)
);

create table teacher (
	TeacherID int NOT NULL AUTO_INCREMENT,
    Title TITLE default 'N/A',
    UserID int reference users(UserId),
    PRIMARY KEY (TeacherID)
);

create table subject (
	Abbreviation varchar(255) NOT NULL,
    Name varchar(255),
    Description varchar(255),
    ECTS varchar(255),
    CoursePoints int default 0,
    Language LANGUAGE default 'Czech',
    IsMandatory boolean default false,
    PRIMARY KEY (Abbreviation)
);

create table gradedActivity (
	ActivityId int NOT NULL AUTO_INCREMENT,
    SubjectAbbreviation varchar(255) NOT NULL,
    Name varchar(255),
    Description varchar(255),
    MinPoints int default 0,
    MaxPoints int default 0,
    PRIMARY KEY (ActivityId),
    FOREIGN KEY (SubjectAbbreviation) REFERENCES subject(Abbreviation)
);

create table enrollement (
	EnrollementId int NOT NULL AUTO_INCREMENT,
    SubjectAbbreviation varchar(255) NOT NULL,
    StudentID int NOT NULL,
    DateEnrolled DATE NOT NULL,
    PRIMARY KEY (EnrollementId),
    FOREIGN KEY (SubjectAbbreviation) REFERENCES subject(Abbreviation),
    FOREIGN KEY (StudentID) REFERENCES student(studentId)
);

create table grade (
	EnrollementId int NOT NULL,
    StudentID int NOT NULL,
    TeacherID int NOT NULL,
    Grade int NOT NULL,
    FOREIGN KEY (EnrollementId) REFERENCES enrollement(EnrollementId),
    FOREIGN KEY (StudentID) REFERENCES student(studentId),
    FOREIGN KEY (TeacherID) REFERENCES teacher(TeacherId)
);

create table teachingSubjects (
    SubjectAbbreviation varchar(255) NOT NULL,
    TeacherID int NOT NULL,
    Position POSITION default 'helper',
    FOREIGN KEY (SubjectAbbreviation) REFERENCES subject(Abbreviation),
    FOREIGN KEY (TeacherID) REFERENCES teacher(TeacherId)
);