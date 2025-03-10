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

@test "Pipes: ls | grep dshlib.c" {
    run "./dsh" <<EOF                
ls | grep dshlib.c
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dshlib.cdsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout: $output"
    echo "Exit Status: $status"
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
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