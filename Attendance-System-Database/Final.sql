CREATE TABLE department (
  dep_id INT AUTO_INCREMENT PRIMARY KEY,
  dep_name NVARCHAR(40) NOT NULL
);

	create table student (
	ssn int primary key,
	student_name nvarchar(40) not null,
	na_id int not null unique,
	email varchar(30) not null unique,
	st_year int not null,
	academic_year INT NOT NULL,
	dep_id int,
	foreign key (dep_id) references department(dep_id)
	);

	CREATE TABLE instructor (
  ins_id INT AUTO_INCREMENT PRIMARY KEY,
  ins_name NVARCHAR(40) NOT NULL,
  email NVARCHAR(50) NOT NULL,  
  na_id int not null unique,
  position NVARCHAR(40) NOT NULL
);



CREATE TABLE course (
  co_id NVARCHAR(10) PRIMARY KEY,
  co_name NVARCHAR(40) NOT NULL,
  co_year INT NOT NULL,
  co_term INT NOT NULL,
  dep_id INT,
  FOREIGN KEY (dep_id) REFERENCES department(dep_id)
);


	create table enroll (
	ssn int NOT NULL,
	co_id NVARCHAR(10),
	foreign key (ssn) references student(ssn),
	foreign key (co_id) references course(co_id)
	);

	create table teach (
	ins_id int,
	co_id NVARCHAR(10),
	foreign key (ins_id) references instructor(ins_id),
	foreign key (co_id) references course(co_id)
	);

	create table attend (
	ssn int,
	co_id NVARCHAR(10),
	atten_date date DEFAULT CURRENT_TIMESTAMP,
    primary key (ssn, co_id, atten_date),
	foreign key (ssn) references student(ssn),
	foreign key (co_id) references course(co_id)
	);

	create table course_schedule (
	co_id NVARCHAR(10),
	schedule_date date,
	foreign key (co_id) references course(co_id)
	);



	create table absent_days (
	ssn int,
	co_id NVARCHAR(10),
	num_absent int,
	primary key (ssn, co_id),
	foreign key (ssn) references student(ssn),
	foreign key (co_id) references course(co_id)
	);


	CREATE TABLE pending_warnings (
	  ssn INT,
	  co_id NVARCHAR(10),
	  state ENUM('first_warning', 'second_warning', 'suspended'),
	  processed TINYINT NOT NULL DEFAULT 0,
	  confirmation_sent TINYINT DEFAULT 0,
	  FOREIGN KEY (ssn) REFERENCES student(ssn),
	  FOREIGN KEY (co_id) REFERENCES course(co_id)
	);

	CREATE TABLE warnings (
	  ssn INT,
	  co_id NVARCHAR(10),
	  state ENUM('first_warning', 'second_warning', 'suspended'),
	  FOREIGN KEY (ssn) REFERENCES student(ssn),
	  FOREIGN KEY (co_id) REFERENCES course(co_id)
	);

	CREATE TABLE historical_student_records (
	  historicalSsn VARCHAR(20) PRIMARY KEY,
	  student_name NVARCHAR(40) NOT NULL,
	  email VARCHAR(30) NOT NULL,
	  na_id INT NOT NULL,
	  st_year INT NOT NULL,
	  dep_id INT
	);

CREATE TABLE historical_course_records (
  historicalSsn VARCHAR(20),
  historicalCo_id VARCHAR(20),
  course_name NVARCHAR(40) NOT NULL,
  num_absent INT,
  num_attended_days INT,
  FOREIGN KEY (historicalSsn) REFERENCES historical_student_records(historicalSsn)
);

CREATE TABLE studentssn_history (
  id INT AUTO_INCREMENT PRIMARY KEY,
  na_id INT NOT NULL,
  academic_year INT NOT NULL,
  historical_ssn VARCHAR(20) NOT NULL
);

CREATE TABLE pending_ill_reports (
  report_id INT AUTO_INCREMENT PRIMARY KEY,
  ssn INT NOT NULL,
  co_id NVARCHAR(10),
  date_of_absent date,
  report_text TEXT NOT NULL,
  attachment VARCHAR(255),
  state TINYINT DEFAULT 0,
  FOREIGN KEY (ssn) REFERENCES student (ssn),
  FOREIGN KEY (co_id) REFERENCES course (co_id)
);

CREATE TABLE admin (
  admin_id INT AUTO_INCREMENT PRIMARY KEY,
  username VARCHAR(50) NOT NULL UNIQUE,
  pass VARCHAR(255) NOT NULL
);










