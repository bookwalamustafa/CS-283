#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Basic: Shell starts and accepts input" {
    run "./dsh" <<EOF
echo "Hello, dsh!"
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "Hello, dsh!" ]]
}

@test "Redirection: ls > test_output.txt" {
    run "./dsh" <<EOF
ls > test_output.txt
EOF

    [ "$status" -eq 0 ]
    [ -s "test_output.txt" ]
}

@test "Redirection: cat < dshlib.h" {
    run "./dsh" <<EOF
cat < dshlib.h
EOF

    [[ "$output" =~ "#ifndef __DSHLIB_H__" ]]  
    [ "$status" -eq 0 ]
}

@test "Redirection: echo test >> append_test.txt" {
    run "./dsh" <<EOF
echo "Appending to file" >> append_test.txt
EOF

    [ "$status" -eq 0 ]
    grep -q "Appending to file" append_test.txt
}

@test "Built-in: cd command" {
    run "./dsh" <<EOF
cd /tmp
pwd
EOF

    [[ "$output" =~ "/tmp" ]]
    [ "$status" -eq 0 ]
}

@test "Built-in: exit command" {
    run "./dsh" <<EOF
exit
EOF

    [ "$status" -eq 0 ]
}

@test "Multiple Commands: echo + ls + pwd" {
    run "./dsh" <<EOF
echo "Test Start"
ls
pwd
echo "Test End"
EOF

    [[ "$output" =~ "Test Start" ]]
    [[ "$output" =~ "Test End" ]]
    [ "$status" -eq 0 ]
}

@test "Whitespace Handling: Trims spaces properly" {
    run "./dsh" <<EOF
   echo    "Trimmed Output"    
EOF

    [[ "$output" =~ "Trimmed Output" ]]
    [ "$status" -eq 0 ]
}

@test "Quoted Arguments: echo 'Hello World'" {
    run "./dsh" <<EOF
echo "Hello   World"
EOF

    [[ "$output" =~ "Hello   World" ]] 
    [ "$status" -eq 0 ]
}

@test "Complex Piping: echo and wc" {
    run "./dsh" <<EOF
echo "Hello world" | wc -w
EOF

    [[ "$output" =~ "2" ]]
    [ "$status" -eq 0 ]
}

@test "Background Execution: sleep & exit" {
    run "./dsh" <<EOF
sleep 2 &
exit
EOF

    [ "$status" -eq 0 ]
}

@test "Pipes: check if ls output can be filtered with grep" {
    run ./dsh <<EOF                
ls | grep dshlib.c
EOF
    [ "$status" -eq 0 ]
}

@test "Pipes: check if multiple commands in a pipeline work" {
    run ./dsh <<EOF                
ls | grep dshlib | wc -l
EOF
    [ "$status" -eq 0 ]
}

@test "Redirection: check if output redirection works" {
    run ./dsh <<EOF                
echo "hello world" > testfile
EOF
    [ "$status" -eq 0 ]
    [ -f testfile ] # Check if file was created
}

@test "Redirection: check if input redirection works" {
    echo "hello world" > testinput
    run ./dsh <<EOF                
cat < testinput
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in: check if exit command works" {
    run ./dsh <<EOF                
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Server: check if stop-server command shuts down server" {
    ./dsh -s &  # Start server in background
    sleep 1  
    run ./dsh -c <<EOF
stop-server
EOF
    sleep 1  # Allow time for shutdown
    ! ps aux | grep "[d]sh -s"  # Check if process is gone
}

@test "Server: check if server starts without crashing" {
    ./dsh -s &  # Start server in background
    sleep 1  # Allow time for startup
    ps aux | grep "[d]sh -s"  # Check if process exists
    kill $!  # Kill server process
}

@test "Client: check if client connects to server" {
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    ./dsh -s &  
    sleep 1  
    run ./dsh -c <<EOF                
echo "test"
EOF
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    [[ "$output" == *"test"* ]]
}

@test "Remote: check if client executes commands remotely" {
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    ./dsh -s &  
    sleep 1  
    run ./dsh -c <<EOF                
echo "remote test"
EOF
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    [[ "$output" == *"remote test"* ]]
}

@test "Remote: check if pipes work remotely" {
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    ./dsh -s &  
    sleep 1  
    run ./dsh -c <<EOF                
ls | wc -l
EOF
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    [ "$status" -eq 0 ]
}

@test "EOF: check if client handles EOF correctly" {
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    ./dsh -s &  
    sleep 1  
    run ./dsh -c <<EOF                
echo "EOF test"
EOF
    ps aux | grep "[d]sh -s" | awk '{print $2}' | xargs kill -9 2>/dev/null || true
    [[ "$output" == *"EOF test"* ]]
}
