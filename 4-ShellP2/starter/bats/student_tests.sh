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

@test "Runs ls without errors" {
    run ./dsh <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Change directory to /tmp" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    [[ "$output" == *"/tmp"* ]]
}

@test "cd with no arguments should not change directory" {
    original_dir=$(pwd)
    run ./dsh <<EOF
cd
pwd
EOF
    [[ "$output" == *"$original_dir"* ]]
}

@test "Exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Handles quoted spaces properly" {
    run ./dsh <<EOF
echo "  hello    world  "
EOF
    [[ "$output" == *"  hello    world  "* ]]
}

@test "Ignores extra spaces between words" {
    run ./dsh <<EOF
echo      hello       world
EOF
    [[ "$output" == *"hello world"* ]]
}

@test "Handles long command inputs" {
    run ./dsh <<EOF
echo $(printf 'a%.0s' {1..200})
EOF
    [ "$status" -eq 0 ]
}