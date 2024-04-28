-- Drop all tables
DROP TABLE users CASCADE CONSTRAINTS;
DROP TABLE students CASCADE CONSTRAINTS;
DROP TABLE teachers CASCADE CONSTRAINTS;
DROP TABLE subjects CASCADE CONSTRAINTS;
DROP TABLE gradedActivity CASCADE CONSTRAINTS;
DROP TABLE enrollment CASCADE CONSTRAINTS;
DROP TABLE grades CASCADE CONSTRAINTS;
DROP TABLE teachingSubjects CASCADE CONSTRAINTS;

-- We chose to split the generalisation/specialisation into 3 tables.
-- That decisionwas made based on the fact that our specialisation is disjunctive
-- (we don't take into consideration cases where a PhD student is also a teacher)
-- and partial (we can have an admin who is neither a student nor a teacher).
-- This implementation also makes sense for login reasons, as all the vital data is in one table,
-- while the rest is in their corresponding subtables.
CREATE TABLE users (
    userID INT GENERATED ALWAYS AS IDENTITY NOT NULL,

    name VARCHAR(255) NOT NULL,
    surname VARCHAR(255) NOT NULL,
    email VARCHAR(255),
    role VARCHAR(255) DEFAULT 'student' CHECK (role IN ('student', 'teacher')),
    birthNumber VARCHAR(10) NOT NULL, -- added for CHECK, not in ERD

    -- This CHECK is for checking the entered birth number
    -- It does basic checks like
    --   * birth number length is 9 or 10
    --   * month is between 1 and 12
    --   * day is between 1 and 31
    --   * birth number is divisible by 11
    CONSTRAINT birthNumber CHECK(
        (LENGTH(birthNumber) = 9 OR LENGTH(birthNumber) = 10) AND
        SUBSTR(birthNumber, 3, 2) BETWEEN 1 AND 12 AND
        SUBSTR(birthNumber, 5, 2) BETWEEN 1 AND 31 AND
        MOD(TO_NUMBER(SUBSTR(birthNumber, 1, 9)), 11) = TO_NUMBER(SUBSTR(birthNumber, 10, 1))
    ),
    PRIMARY KEY (UserID)
);


CREATE TABLE students (
    userID INT PRIMARY KEY NOT NULL,

    yearEnrolled INT NOT NULL,
    creditsGained INT DEFAULT 0,
    creditsEnrolled INT DEFAULT 0,

    FOREIGN KEY (userID) REFERENCES users(userID)
);

CREATE TABLE teachers (
    userID INT PRIMARY KEY NOT NULL,

    title VARCHAR(100) DEFAULT 'N/A',

    FOREIGN KEY (userID) REFERENCES users(userID)
);

CREATE TABLE subjects (
    abbreviation VARCHAR(5) NOT NULL CHECK (LENGTH(abbreviation) = 3 OR LENGTH(abbreviation) = 4),

    name VARCHAR(255),
    description VARCHAR(2550),
    ECTS INT NOT NULL,
    coursePoints INT DEFAULT 100,
    language VARCHAR(100) DEFAULT 'Czech',
    isMandatory NUMBER(1,0) DEFAULT 0, -- Cannot use Boolean in PL SQL for some reason => 0 is false

    PRIMARY KEY (abbreviation)
);

CREATE TABLE gradedActivity (
    activityID INT GENERATED ALWAYS AS IDENTITY NOT NULL,

    subjectAbbreviation VARCHAR(5) NOT NULL,
    name VARCHAR(255),
    description VARCHAR(2550),
    minPoints INT DEFAULT 0,   -- min points required to still be able to pass the subject (eg final exam must get at least 25 pts.)
    maxPoints INT DEFAULT 100, -- max points student can get from this graded act.

    PRIMARY KEY (activityID),
    FOREIGN KEY (subjectAbbreviation) REFERENCES subjects(abbreviation)
);

CREATE TABLE enrollment (
    enrollmentID INT GENERATED ALWAYS AS IDENTITY NOT NULL,

    studentID INT NOT NULL,
    subjectAbbreviation VARCHAR(255) NOT NULL,
    dateEnrolled DATE NOT NULL,
    totalPoints INT DEFAULT 0,

    PRIMARY KEY (enrollmentID),
    FOREIGN KEY (subjectAbbreviation) REFERENCES subjects(abbreviation),
    FOREIGN KEY (studentID) REFERENCES students(userID)
);

CREATE TABLE grades (
    enrollmentID INT NOT NULL,
    activityID INT NOT NULL,
    teacherID INT NOT NULL,
    points INT DEFAULT 0 NOT NULL, -- Renamed from ER's grade to points. Make more sense imo.

    PRIMARY KEY (enrollmentID, activityID), -- Once per student per activity
    FOREIGN KEY (enrollmentID) REFERENCES enrollment(enrollmentID),
    FOREIGN KEY (activityID) REFERENCES gradedActivity(activityID),
    FOREIGN KEY (teacherID) REFERENCES teachers(userID)
);

CREATE TABLE teachingSubjects (
    teacherID INT NOT NULL,
    subjectAbbreviation VARCHAR(255) NOT NULL,
    position VARCHAR(100) DEFAULT 'TA' NOT NULL CHECK (position IN ('TA', 'Guarantor', 'Lecturer')), -- Teaching Assistant
    
    FOREIGN KEY (subjectAbbreviation) REFERENCES subjects(abbreviation),
    FOREIGN KEY (teacherID) REFERENCES teachers(userID)
);

-- TRIGGERs
------------------------------------------------------------------------------------------
-- This trigger is used to update the total points of a student when a new grade is inserted
CREATE OR REPLACE TRIGGER Add_Total_Points
    AFTER INSERT OR UPDATE ON grades
    FOR EACH ROW
BEGIN
    UPDATE enrollment
    SET totalPoints = totalPoints + :new.points
    WHERE enrollmentID = :new.enrollmentID;
END;
/
-- to see this trigger in action, look at "Grades INSERTs", where there are points added. You can view it works by running this query:
-- SELECT * FROM enrollment;
-- Here is an INSERT to try out on its own:
-- INSERT INTO grades(enrollmentID, activityID, teacherID, points)
-- VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Michal') AND subjectAbbreviation = 'IDS'), 
-- (SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Půlsemestrální zkouška'), (SELECT userID FROM users WHERE surname = 'Bartík'), 50);

------------------------------------------------------------------------------------------
-- This trigger adds credits enrolled after a student enrolls in a subject
CREATE OR REPLACE TRIGGER Add_Credits_Enrolled
    AFTER INSERT ON enrollment
    FOR EACH ROW
DECLARE
    credits INT;
BEGIN
    -- Retrieve the ECTS value for the subject
    SELECT ECTS INTO credits
    FROM subjects
    WHERE abbreviation = :new.subjectAbbreviation;

    -- Update the creditsEnrolled
    UPDATE students
    SET creditsEnrolled = creditsEnrolled + credits
    WHERE userID = :new.studentID;
END;
/

------------------------------------------------------------------------------------------
-- This trigger grants a student ECTS for a subject if they gained 50 or more points already
CREATE OR REPLACE TRIGGER Check_Points_Credits
    AFTER INSERT OR UPDATE ON enrollment
    FOR EACH ROW
DECLARE
    credits INT;
BEGIN
    IF :new.totalPoints >= 50 THEN
        -- Retrieve the ECTS value for the subject
        SELECT ECTS INTO credits
        FROM subjects
        WHERE abbreviation = :new.subjectAbbreviation;

        -- Update the creditsGained
        UPDATE students
        SET creditsGained = creditsGained + credits
        WHERE userID = :new.studentID;
    END IF;
END;
/

-- See how it works by uncommenting the following 4 lines:
-- INSERT INTO grades(enrollmentID, activityID, teacherID, points)
-- VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Michal') AND subjectAbbreviation = 'IDS'), 
-- (SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Půlsemestrální zkouška'), (SELECT userID FROM users WHERE surname = 'Bartík'), 50);
-- SELECT * FROM STUDENTS;

-- PROCEDUREs
------------------------------------------------------------------------------------------

-- This procedure gets students that received more than 50 points in a subject
CREATE OR REPLACE PROCEDURE Students_That_Passed_Subject(SubjectAbb IN VARCHAR) AS
    CURSOR c_student IS
        SELECT DISTINCT u.name, u.surname
        FROM users u
        INNER JOIN students s ON u.userID = s.userID
        INNER JOIN enrollment e ON s.userID = e.studentID
        INNER JOIN grades g ON e.enrollmentID = g.enrollmentID
        WHERE e.subjectAbbreviation = SubjectAbb
        GROUP BY u.name, u.surname
        HAVING SUM(g.points) > 50;

    c_student_row c_student%ROWTYPE;
    loop_count NUMBER := 0;
BEGIN
    OPEN c_student;

    LOOP
        FETCH c_student INTO c_student_row;
        EXIT WHEN c_student%NOTFOUND;
        IF loop_count = 0 THEN
            DBMS_OUTPUT.PUT_LINE('Students with more than 50 total points in subject ' || SubjectAbb || ':');
        END IF;
        loop_count := loop_count + 1;
        DBMS_OUTPUT.PUT_LINE(c_student_row.name || ' ' || c_student_row.surname);
    END LOOP;

    IF loop_count = 0 THEN
        RAISE NO_DATA_FOUND;
    END IF;

    CLOSE c_student;
    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            DBMS_OUTPUT.PUT_LINE('No students found for subject ' || SubjectAbb);
        WHEN OTHERS THEN
            DBMS_OUTPUT.PUT_LINE('An error occurred');
END;
/
EXEC Students_That_Passed_Subject('IDS');
EXEC Students_That_Passed_Subject('IPK');

-- This cursor generates a report for a student (like el. index in IS VUT)
CREATE OR REPLACE PROCEDURE Generate_Report(StudID IN INT) AS
    CURSOR c_student IS
        SELECT u.name, u.surname, s.yearEnrolled, s.creditsGained, s.creditsEnrolled, e.subjectAbbreviation, e.totalPoints, sub.ECTS
        FROM users u
        INNER JOIN students s ON u.userID = s.userID
        INNER JOIN enrollment e ON s.userID = e.studentID
        INNER JOIN subjects sub ON e.subjectAbbreviation = sub.abbreviation
        WHERE s.userID = StudID;

    c_student_row c_student%ROWTYPE;
    loop_count NUMBER := 0;
BEGIN
    OPEN c_student;

    LOOP
        FETCH c_student INTO c_student_row;
        EXIT WHEN c_student%NOTFOUND;
        IF loop_count = 0 THEN
            DBMS_OUTPUT.PUT_LINE('Report for student ' || c_student_row.name || ' ' || c_student_row.surname);
            DBMS_OUTPUT.PUT_LINE(chr(9) || 'Year enrolled: ' || c_student_row.yearEnrolled);
            DBMS_OUTPUT.PUT_LINE(chr(9) || 'Credits gained: ' || c_student_row.creditsGained);
            DBMS_OUTPUT.PUT_LINE(chr(9) || 'Credits enrolled: ' || c_student_row.creditsEnrolled);
            DBMS_OUTPUT.PUT_LINE(chr(9) || 'Subjects enrolled and points:');
        END IF;
        loop_count := loop_count + 1;
        DBMS_OUTPUT.PUT_LINE(chr(9) || chr(9) || c_student_row.subjectAbbreviation || ': ' || c_student_row.totalPoints || ' points (' || c_student_row.ECTS || ' ECTS)');
    END LOOP;

    IF loop_count = 0 THEN
        RAISE NO_DATA_FOUND;
    END IF;

    CLOSE c_student;
    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            DBMS_OUTPUT.PUT_LINE('No students found for student ID ' || StudID);
        WHEN OTHERS THEN
            DBMS_OUTPUT.PUT_LINE('An error occurred');
END;
/

EXEC Generate_Report(1);



-- INSERTs
------------------------------------------------------------------------------------------
-- Student INSERTs
------------------------------------------------------------------------------------------
INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Michal', 'Pavlíček', 'xpavlim00@fit.vutbr.cz', 'student', '0212155262');
INSERT INTO students (yearEnrolled, userID)
VALUES (2023, (SELECT userID FROM users WHERE birthNumber = '0212155262'));

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Tomáš', 'Vlach', 'xvlach24@fit.vutbr.cz', 'student', '0212155251');
INSERT INTO students (yearEnrolled, userID)
VALUES (2001, (SELECT userID FROM users WHERE birthNumber = '0212155251'));

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Jiří', 'Hojný', 'xhojny85@fit.vutbr.cz', 'student', '0212155240');
INSERT INTO students (yearEnrolled, userID)
VALUES (1999, (SELECT userID FROM users WHERE birthNumber = '0212155240'));

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Vladimír', 'Pospíšil', 'xpospi08@fit.vutbr.cz', 'student', '0212155229');
INSERT INTO students (yearEnrolled, userID)
VALUES (2001, (SELECT userID FROM users WHERE birthNumber = '0212155229'));

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('František', 'Svoboda', 'xsvobo88@fit.vutbr.cz', 'student', '0212155218');
INSERT INTO students (yearEnrolled, userID)
VALUES (2000, (SELECT userID FROM users WHERE birthNumber = '0212155218'));
------------------------------------------------------------------------------------------
-- Teacher INSERTs
------------------------------------------------------------------------------------------
INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Vladimír', 'Bartík', 'bartik@fit.vutbr.cz', 'teacher', '0212155273');
INSERT INTO teachers (title, userID)
SELECT 'Ing., Ph.D.', userID
FROM users
WHERE birthNumber = '0212155273';

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Marek', 'Rychlý', 'rychly@fit.vutbr.cz', 'teacher', '0212155284');
INSERT INTO teachers (title, userID)
SELECT 'RNDr., Ph.D.', userID
FROM users
WHERE birthNumber = '0212155284';

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Miloš', 'Musil', 'imusilm@fit.vutbr.cz', 'teacher', '0212155295');
INSERT INTO teachers (title, userID)
SELECT 'Ing., Ph.D.', userID
FROM users
WHERE birthNumber = '0212155295';

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Ondřej', 'Ryšavý', 'rysavy@fit.vutbr.cz', 'teacher', '0212155306');
INSERT INTO teachers (title, userID)
SELECT 'doc. Ing, Ph.D.', userID
FROM users
WHERE birthNumber = '0212155306';

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Vladimír', 'Veselý', 'veselyv@fit.vutbr.cz', 'teacher', '0212155317');
INSERT INTO teachers (title, userID)
SELECT 'Ing., Ph.D.', userID
FROM users
WHERE birthNumber = '0212155317';
------------------------------------------------------------------------------------------
-- Subjects INSERTs
------------------------------------------------------------------------------------------
INSERT INTO subjects (abbreviation, name, description, ECTS, isMandatory)
VALUES ('IDS', 'Databázové systémy', 'Předmět zabývající se databázemi', 5, 1);

INSERT INTO subjects (abbreviation, name, description, ECTS, isMandatory)
VALUES ('IPK', 'Počítačové komunikace a sítě', 'Předmět zabývající se síťovými protokoly', 4, 1);

INSERT INTO subjects (abbreviation, name, description, ECTS)
VALUES ('IJC', 'Jazyk C', 'Předmět prohlubující znalosti jazyka C', 5);

INSERT INTO subjects (abbreviation, name, description, ECTS, language, isMandatory)
VALUES ('IFJa', 'Formal Languages and Compilers', 'English version of subject IFJ', 4, 'English', 1);
------------------------------------------------------------------------------------------
-- GradedActivity INSERTs
------------------------------------------------------------------------------------------
INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ('IDS', 'Projekt část 1.', 'Vytvořit ER Diagram', 0, 5);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ('IDS', 'Projekt část 2.', 'Vytvořit základní SQL soubor', 0, 5);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ('IDS', 'Půlsemestrální zkouška', 'Do 6. prezentace včetně', 0, 20);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ('IDS', 'Zkouška', 'Finální zkouška', 25, 60);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ('IPK', 'Projekt 1.', 'UDP/TCP Chat Client using the protocol IPK24Chat', 0, 20);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ('IPK', 'Projekt 2.', 'A network sniffer', 0, 20);
------------------------------------------------------------------------------------------
-- Enrollment INSERTs
------------------------------------------------------------------------------------------
INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IDS', (SELECT userID FROM users WHERE name = 'Michal' ), TO_DATE('12-Jan-2024'));

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IPK', (SELECT userID FROM users WHERE name = 'Michal' ), TO_DATE('01-Jan-2024'));

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IFJa', (SELECT userID FROM users WHERE name = 'Michal' ), TO_DATE('30-Dec-2023'));

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IDS', (SELECT userID FROM users WHERE name = 'Tomáš' ), TO_DATE('01-Jan-2024'));

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IJC', (SELECT userID FROM users WHERE name = 'Tomáš' ), TO_DATE('01-Jan-2024'));

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IFJa', (SELECT userID FROM users WHERE name = 'Tomáš' ), TO_DATE('01-Jan-2024'));
------------------------------------------------------------------------------------------
-- Grades INSERTs
------------------------------------------------------------------------------------------
INSERT INTO grades(enrollmentID, activityID, teacherID, points)
-- insert all values with select 
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Michal') AND subjectAbbreviation = 'IDS'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Projekt část 1.'), (SELECT userID FROM users WHERE surname = 'Bartík'), 5);

INSERT INTO grades(enrollmentID, activityID, teacherID, points)
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Tomáš') AND subjectAbbreviation = 'IDS'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Projekt část 1.'), (SELECT userID FROM users WHERE surname = 'Bartík'), 4);

INSERT INTO grades(enrollmentID, activityID, teacherID, points)
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Michal') AND subjectAbbreviation = 'IDS'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Projekt část 2.'), (SELECT userID FROM users WHERE surname = 'Bartík'), 3);

INSERT INTO grades(enrollmentID, activityID, teacherID, points)
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Tomáš') AND subjectAbbreviation = 'IDS'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Projekt část 2.'), (SELECT userID FROM users WHERE surname = 'Bartík'), 4);

INSERT INTO grades(enrollmentID, activityID, teacherID, points)
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Michal') AND subjectAbbreviation = 'IDS'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Půlsemestrální zkouška'), (SELECT userID FROM users WHERE surname = 'Bartík'), 50);

INSERT INTO grades(enrollmentID, activityID, teacherID, points)
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Tomáš') AND subjectAbbreviation = 'IDS'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IDS' AND name = 'Půlsemestrální zkouška'), (SELECT userID FROM users WHERE surname = 'Bartík'), 55);


INSERT INTO grades(enrollmentID, activityID, teacherID, points)
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = (SELECT userID FROM users WHERE name = 'Michal') AND subjectAbbreviation = 'IPK'), 
(SELECT activityID FROM gradedActivity WHERE subjectAbbreviation = 'IPK' AND name = 'Projekt 1.'), (SELECT userID FROM users WHERE surname = 'Bartík'), 17);
------------------------------------------------------------------------------------------
-- TeachingSubjects INSERTs
------------------------------------------------------------------------------------------
INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IDS', (SELECT userID FROM users WHERE surname = 'Bartík'), 'Guarantor');

INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IDS', (SELECT userID FROM users WHERE surname = 'Rychlý'), 'Lecturer');

INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IDS', (SELECT userID FROM users WHERE surname = 'Musil'), 'Lecturer');

INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IPK', (SELECT userID FROM users WHERE surname = 'Ryšavý'), 'Guarantor');

INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IPK', (SELECT userID FROM users WHERE surname = 'Veselý'), 'Lecturer');
------------------------------------------------------------------------------------------

-- SELECT queries
------------------------------------------------------------------------------------------
---- Two join SELECT queries
------------------------------------------------------------------------------------------
-- SELECT titles and names of teachers
SELECT teachers.title, users.name, users.surname 
FROM users 
INNER JOIN teachers ON users.userID = teachers.userID;

-- SELECT names and dates of enrolment of students
SELECT users.name, users.surname, students.yearenrolled 
FROM users 
INNER JOIN students ON users.userid = students.userid;

-- SELECT all graded activities for subject IDS
SELECT subjects.abbreviation, subjects.name AS subject_name,
       gradedactivity.name AS graded_assignment, gradedactivity.description,
       gradedactivity.minpoints, gradedactivity.maxpoints
FROM gradedactivity
INNER JOIN subjects ON gradedactivity.subjectabbreviation = subjects.abbreviation
WHERE gradedactivity.subjectabbreviation = 'IDS';


------------------------------------------------------------------------------------------
---- Three join SELECT queries
------------------------------------------------------------------------------------------
-- SELECT names, subject abbreviations and date enrollments of students
SELECT users.name, users.surname, enrollment.subjectabbreviation, enrollment.dateenrolled 
FROM users 
INNER JOIN students ON users.userid = students.userid 
INNER JOIN enrollment ON students.userid = enrollment.studentid;

-- SELECT which teachers teach which subject
SELECT users.name, users.surname, subjects.abbreviation
FROM users
INNER JOIN teachers ON users.userid = teachers.userid
INNER JOIN teachingsubjects ON teachers.userid = teachingsubjects.teacherid
INNER JOIN subjects ON teachingsubjects.subjectabbreviation = subjects.abbreviation;

------------------------------------------------------------------------------------------
---- GROUP BY queries
------------------------------------------------------------------------------------------
-- SELECT subject abbreviations and amount of students enrolled in a subject
SELECT enrollment.subjectabbreviation, COUNT(users.userid) AS num_students
FROM users 
INNER JOIN students ON users.userid = students.userid 
INNER JOIN enrollment ON students.userid = enrollment.studentid 
GROUP BY enrollment.subjectabbreviation;

-- SELECT graded activities (projects, exams) and their average grade
SELECT gradedactivity.name, COUNT(grades.points) AS num_grades, AVG(grades.points) AS avg_grade
FROM grades
LEFT JOIN gradedactivity ON grades.activityid = gradedactivity.activityid
GROUP BY gradedactivity.name;

-- SELECT subjects where there is 2 or less teachers teaching it
SELECT subjects.abbreviation, COUNT(teachingsubjects.teacherid) AS num_teachers
FROM subjects
LEFT JOIN teachingsubjects ON subjects.abbreviation = teachingsubjects.subjectabbreviation
GROUP BY subjects.abbreviation
HAVING COUNT(teachingsubjects.teacherid) <= 2;

------------------------------------------------------------------------------------------
---- EXISTS queries
------------------------------------------------------------------------------------------
-- SELECT all students, that have more than 10 points in any graded activity
SELECT users.name, users.surname
FROM users
INNER JOIN students ON users.userID = students.userID
INNER JOIN enrollment ON students.userID = enrollment.studentID
WHERE EXISTS (
    SELECT *
    FROM grades
    WHERE enrollment.enrollmentID = grades.enrollmentID
    AND grades.points > 10
);

-- SELECT all usesrs that are students and are enrolled in a course
SELECT users.name, users.surname
FROM users
WHERE EXISTS (
    SELECT *
    FROM students
    WHERE users.userID = students.userid
)
AND EXISTS (
    SELECT *
    FROM enrollment 
    WHERE users.userID = enrollment.studentID
    );

------------------------------------------------------------------------------------------
---- IN queries
------------------------------------------------------------------------------------------
-- SELECT all users and count the amount of mandatory subjects they are enrolled in
SELECT users.name, users.surname, COUNT(enrollment.subjectabbreviation) AS num_mandatory_subjects
FROM users
INNER JOIN students ON users.userID = students.userID
INNER JOIN enrollment ON students.userID = enrollment.studentID
WHERE enrollment.subjectabbreviation IN (
    SELECT subjects.abbreviation
    FROM subjects
    WHERE subjects.ismandatory = 1
)
GROUP BY users.name, users.surname;

-- SELECT all subjects for which the student with surname "Pavlíček" has enrolled in in the year 2024
SELECT subjects.abbreviation, subjects.name
FROM subjects
WHERE subjects.abbreviation IN (
    SELECT enrollment.subjectabbreviation
    FROM enrollment
    INNER JOIN students ON enrollment.studentID = students.userID
    INNER JOIN users ON students.userID = users.userID
    WHERE users.surname = 'Pavlíček'
    AND TO_CHAR(enrollment.dateenrolled, 'YYYY') = '2024'
);
