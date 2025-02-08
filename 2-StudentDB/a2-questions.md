## Assignment 2 Questions

#### Directions

Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

   > **Answer**: I believe externalizing the `get_student()` function is a good design choice. It makes the code cleaner anmd more maintainable since functions like `add_student()`, `del_student()`, and `print_db()` all rely on retririeving student records. Having a dedicated function helps eliminate redundant code, reduces the risk of bugs, and centralizes any changes related to student retrieval. It also improves readabiltity by keeping the retrieval logic in one well-defined place.
   >
2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

   ```c
   int get_student(int fd, int id, student_t *s);
   ```
   Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language.

   In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

   ```c
   //Lookup student from the database
   // IF FOUND: return pointer to student data
   // IF NOT FOUND: return NULL
   student_t *get_student(int fd, int id){
       student_t student;
       bool student_found = false;

       //code that looks for the student and if
       //found populates the student structure
       //The found_student variable will be set
       //to true if the student is in the database
       //or false otherwise.

       if (student_found)
           return &student;
       else
           return NULL;
   }
   ```
   Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime?

   > **ANSWER:** The issue with this implementation is that it returns a pointer to a local variable (student) that's stored on the stack. In C, once the function returns, the stack frame is deallocated, making the pointer invalid. Accessing it can lead to undefined behavior - sometimes it might seem to work, but it can cause random data issues, crashes, or even security vulnerabilities. This kind of bug is tricky because it doesn't always fail right away, making it hard to spot.
   >
3. Another way the `get_student(...)` function could be implemented is as follows:

   ```c
   //Lookup student from the database
   // IF FOUND: return pointer to student data
   // IF NOT FOUND or memory allocation error: return NULL
   student_t *get_student(int fd, int id){
       student_t *pstudent;
       bool student_found = false;

       pstudent = malloc(sizeof(student_t));
       if (pstudent == NULL)
           return NULL;

       //code that looks for the student and if
       //found populates the student structure
       //The found_student variable will be set
       //to true if the student is in the database
       //or false otherwise.

       if (student_found){
           return pstudent;
       }
       else {
           free(pstudent);
           return NULL;
       }
   }
   ```
   In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.

   > **ANSWER:** This implementation works because `malloc()` allocates memory on the heap, so the data persists after the function returns. however, it comes with some issues. First, the caller has to free the memory, and if they forget, it leads to memory leaks-especially problematic in long-running programs. Second, it adds complexity since memory management is now the caller's responsibility, increasing the risk of errors. Lastly, frequent dynamic allocations can slow thnigs down due to the overhead of managing heap memory. While it offers flexibility, it requires careful handling to avoid memory-related issues.
   >
4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

   ```bash
   > ./sdbsc -a 1 john doe 345
   > ls -l ./student.db
       -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
   > du -h ./student.db
       4.0K    ./student.db
   > ./sdbsc -a 3 jane doe 390
   > ls -l ./student.db
       -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
   > du -h ./student.db
       4.0K    ./student.db
   > ./sdbsc -a 63 jim doe 285 
   > du -h ./student.db
       4.0K    ./student.db
   > ./sdbsc -a 64 janet doe 310
   > du -h ./student.db
       8.0K    ./student.db
   > ls -l ./student.db
       -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
   ```
   For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

   - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64?

     > **ANSWER:** The difference between the `ls` and `du` outputs is because of how Linux handles sparse files. When using `lseek()` to skip over parts of a file without writing data, it creates "holes" - unallocated spaces that still count toward the file size reported by `ls`, but don't take up actual disk space, which `du` reflects.
     >
   - Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64?

     > **ANSWER:** The disk storage stayed at 4K when adding students with IDs 1, 3, and 63 because they all fit within the same 4KB file system block. As long as the data stays within that block, no extra disk space is needed. When I added student ID=64, the file grew beyond the first 4KB block, so the system had to allocate another block, increasing the disk usage to 8K.
     >
   - Now lets add one more student with a large student ID number  and see what happens:

     ```bash
     > ./sdbsc -a 99999 big dude 205 
     > ls -l ./student.db
     -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
     > du -h ./student.db
     12K     ./student.db
     ```
     We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

     > **ANSWER:** When I added the student with ID=99999, the file size increased significantly because lseek() created a large gap in the file, leaving empty spaces called "holes." However, du only showed 12K of actual disk usage because the file system only allocates space for the data that's actually written, not the empty gaps. This is due to sparse file optimization, which helps save storage space.
     >
