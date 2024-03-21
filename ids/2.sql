-- Drop all tables
DROP TABLE users CASCADE CONSTRAINTS;
DROP TABLE students CASCADE CONSTRAINTS;
DROP TABLE teachers CASCADE CONSTRAINTS;
DROP TABLE subjects CASCADE CONSTRAINTS;
DROP TABLE gradedActivity CASCADE CONSTRAINTS;
DROP TABLE enrollment CASCADE CONSTRAINTS;
DROP TABLE grade CASCADE CONSTRAINTS;
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
    birthNumber VARCHAR(10) NOT NULL,

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
    PRIMARY KEY (UserId)
);


CREATE TABLE students (
    userID INT NOT NULL,

    yearEnrolled INT NOT NULL,
    creditsGained INT DEFAULT 0,
    creditsEnrolled INT DEFAULT 0,

    FOREIGN KEY (userID) REFERENCES users(userID)
);


------------------------------------------------------------------------------------------
INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Michal', 'Pavlíček', 'xpavlim00@fit.vutbr.cz', 'student', '0212155262');
-- TODO: Check if userID can be given like this
INSERT INTO students (yearEnrolled, userID)
VALUES (2023, 1);

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Tomáš', 'Vlach', 'xvlach24@fit.vutbr.cz', 'student', '0212155251');
INSERT INTO students (yearEnrolled, userID)
VALUES (2001, (SELECT userID FROM users WHERE birthNumber = '0212155251'));
------------------------------------------------------------------------------------------


CREATE TABLE teachers (
    userID INT NOT NULL,

    title VARCHAR(100) DEFAULT 'N/A',

    FOREIGN KEY (userID) REFERENCES users(userID)
);

------------------------------------------------------------------------------------------
INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Vladimír', 'Bartík', 'bartik@fit.vutbr.cz', 'teacher', '0212155273');
INSERT INTO teachers (title, userID)
VALUES ("Ing., Ph.D.", (SELECT userID FROM users WHERE birthNumber = '0212155273'));

INSERT INTO users (name, surname, email, role, birthNumber)
VALUES ('Marek', 'Rychlý', 'rychly@fit.vutbr.cz', 'teacher', '0212155284');
INSERT INTO teachers (title, userID)
VALUES ("RNDr., Ph.D.", (SELECT userID FROM users WHERE birthNumber = '0212155284'));
------------------------------------------------------------------------------------------

CREATE TABLE subjects (
    abbreviation VARCHAR(5) NOT NULL UNIQUE CHECK (LENGTH(abbreviation) == 3 OR LENGTH(abbreviation) == 4),

    name VARCHAR(255),
    description VARCHAR(2550),
    ECTS INT NOT NULL,
    coursePoints INT DEFAULT 100,
    language VARCHAR(100) DEFAULT 'Czech',
    isMandatory NUMBER(1,0) DEFAULT 0, -- Cannot use Boolean in PL SQL for some reason => 0 is false

    PRIMARY KEY (abbreviation)
);


------------------------------------------------------------------------------------------
INSERT INTO subjects (abbreviation, name, description, ECTS, isMandatory)
VALUES ("IDS", "Databázové systémy", "Předmět zabývající se databázemi", 5, 1);

INSERT INTO subjects (abbreviation, name, description, ECTS, isMandatory)
VALUES ("IPK", "Počítačové komunikace a sítě", "Předmět zabývající se síťovými protokoly", 4, 1);

INSERT INTO subjects (abbreviation, name, description, ECTS)
VALUES ("IJC", "Jazyk C", "Předmět prohlubující znalosti jazyka C", 5);

INSERT INTO subjects (abbreviation, name, description, ECTS, language, isMandatory)
VALUES ("IFJa", "Formal Languages and Compilers", "English version of subject IFJ", 4, 1, "English");
------------------------------------------------------------------------------------------


-- Create gradedActivity table
CREATE TABLE gradedActivity (
    activityId INT GENERATED ALWAYS AS IDENTITY NOT NULL,

    subjectAbbreviation VARCHAR(5) NOT NULL,
    name VARCHAR(255),
    description VARCHAR(2550),
    minPoints INT DEFAULT 0,   -- min points required to still be able to pass the subject (eg final exam must get at least 25 pts.)
    maxPoints INT DEFAULT 100, -- max points student can get from this graded act.

    PRIMARY KEY (activityId)
    FOREIGN KEY (subjectAbbreviation) REFERENCES subjects(abbreviation)
);


------------------------------------------------------------------------------------------
INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ("IDS", "Projekt část 1.", "Vytvořit ER Diagram", 0, 5);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ("IDS", "Projekt část 2.", "Vytvořit základní SQL soubor", 0, 5);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ("IDS", "Půlsemestrální zkouška", "Do 6. prezentace včetně", 0, 20);

INSERT INTO gradedActivity (subjectAbbreviation, name, description, minPoints, maxPoints)
VALUES ("IDS", "Zkouška", "", 25, 60);
------------------------------------------------------------------------------------------


CREATE TABLE enrollment (
    enrollmentID INT GENERATED ALWAYS AS IDENTITY NOT NULL,

    subjectAbbreviation VARCHAR(255) NOT NULL,
    studentID INT NOT NULL,
    dateEnrolled DATE NOT NULL,

    PRIMARY KEY (enrollmentID),
    FOREIGN KEY (subjectAbbreviation) REFERENCES subjects(abbreviation),
    FOREIGN KEY (studentID) REFERENCES students(userID)
);

------------------------------------------------------------------------------------------
INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
-- TODO: Check TO_DATE
VALUES ('IDS', (SELECT userId FROM users WHERE name = 'Michal' ), TO_DATE('12-Jan-2024'))

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IDS', (SELECT userId FROM users WHERE birthNumber = '0212155251' ), TO_DATE('01-Jan-2024'))

INSERT INTO enrollment (subjectAbbreviation, studentID, dateEnrolled)
VALUES ('IPK', (SELECT userId FROM users WHERE birthNumber = '0212155251' ), TO_DATE('01-Jan-2024'))
------------------------------------------------------------------------------------------

-- Create grade table
CREATE TABLE grades (
    enrollmentID INT NOT NULL,
    studentID INT NOT NULL,
    teacherID INT NOT NULL,
    points INT NOT NULL DEFAULT 0, -- Renamed from ER's grade to points. Make more sense imo.

    FOREIGN KEY (enrollmentID) REFERENCES enrollment(enrollmentID),
    FOREIGN KEY (studentID) REFERENCES students(userID),
    FOREIGN KEY (teacherID) REFERENCES teachers(userID)
);

------------------------------------------------------------------------------------------
INSERT INTO grades(enrollmentID, studentID, teacherID, points)
-- TODO: might be wrong again
VALUES ((SELECT enrollmentID FROM enrollment WHERE studentID = 1) 1, 1, 5);
------------------------------------------------------------------------------------------


-- Create teachingSubjects table
CREATE TABLE teachingSubjects (
    subjectAbbreviation VARCHAR(255) NOT NULL,
    teacherID INT NOT NULL,
    position VARCHAR(100) DEFAULT 'TA' NOT NULL CHECK (position IN ('TA', 'Guarantor', 'Lecturer')), -- Teaching Assistant
    
    FOREIGN KEY (subjectAbbreviation) REFERENCES subjects(abbreviation),
    FOREIGN KEY (teacherID) REFERENCES teachers(userID)
);

------------------------------------------------------------------------------------------
INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IDS', (SELECT userID FROM users WHERE surname = 'Bartík'), 'Guarantor');

INSERT INTO teachingSubjects(subjectAbbreviation, teacherID, position)
VALUES ('IDS', (SELECT userID FROM users WHERE surname = 'Rychlý'), 'Lecturer');
------------------------------------------------------------------------------------------