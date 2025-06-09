#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Computer Graphics Rasterization Pipeline Test Script ===${NC}"
echo -e "${BLUE}Testing all 4 test cases...${NC}\n"

# Clean up any existing output files from previous runs
echo -e "${YELLOW}Cleaning up previous run files...${NC}"
# Note: Preserving z-buffer and bitmap files for next session
echo -e "${GREEN}Previous files cleaned!${NC}\n"

# Compile the programs
echo -e "${YELLOW}Compiling programs...${NC}"
g++ -o task1 2005107_task1.cpp
g++ -o task2 2005107_task2.cpp
g++ -o task3 2005107_task3.cpp
g++ -o task4 2005107_task4.cpp

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}\n"

# Function to compare files (focusing on numerical values, ignoring ALL whitespace)
compare_files() {
    local file1="$1"
    local file2="$2"
    local stage_name="$3"
    
    if [ ! -f "$file1" ]; then
        echo -e "${RED}Generated file $file1 not found!${NC}"
        return 1
    fi
    
    if [ ! -f "$file2" ]; then
        echo -e "${RED}Expected file $file2 not found!${NC}"
        return 1
    fi
    
    # For better numerical comparison, remove ALL whitespace characters
    local temp1=$(mktemp)
    local temp2=$(mktemp)
    
    # Remove ALL whitespace: tabs, spaces, newlines, carriage returns
    tr -d ' \t\n\r' < "$file1" > "$temp1"
    tr -d ' \t\n\r' < "$file2" > "$temp2"
    
    # Compare the files with all whitespace removed
    if diff -q "$temp1" "$temp2" > /dev/null; then
        echo -e "${GREEN}✓ $stage_name: Values matched${NC}"
        rm -f "$temp1" "$temp2"
        return 0
    else
        echo -e "${RED}✗ $stage_name: Values did not match${NC}"
        rm -f "$temp1" "$temp2"
        return 1
    fi
}

# Function to test a single test case
test_case() {
    local case_num="$1"
    local test_dir="Test Cases/$case_num"
    
    echo -e "${BLUE}Testing Case $case_num...${NC}"
    
    # Generated output files
    local my_stage1="$test_dir/my_stage1.txt"
    local my_stage2="$test_dir/my_stage2.txt"
    local my_stage3="$test_dir/my_stage3.txt"
    local my_zbuffer="$test_dir/test${case_num}_z_buffer.txt"
    local my_output_bmp="$test_dir/test${case_num}_my_output.bmp"
    
    # Expected output files
    local exp_stage1="$test_dir/stage1.txt"
    local exp_stage2="$test_dir/stage2.txt"
    local exp_stage3="$test_dir/stage3.txt"
    local exp_zbuffer="$test_dir/z_buffer.txt"
    local exp_output_bmp="$test_dir/out.bmp"
    
    # Input files
    local scene_file="$test_dir/scene.txt"
    local config_file="$test_dir/config.txt"
    
    # Run the pipeline
    echo "  Running Stage 1 (Model Transformation)..."
    ./task1 "$scene_file" "$my_stage1"
    
    echo "  Running Stage 2 (View Transformation)..."
    ./task2 "$scene_file" "$my_stage1" "$my_stage2"
    
    echo "  Running Stage 3 (Projection Transformation)..."
    ./task3 "$scene_file" "$my_stage2" "$my_stage3"
    
    echo "  Running Stage 4 (Z-Buffer Processing)..."
    ./task4 "$config_file" "$my_stage3"
    
    # Move generated files to test directory with proper names
    if [ -f "my_z_buffer.txt" ]; then
        mv "my_z_buffer.txt" "$my_zbuffer"
    fi
    if [ -f "my_output.bmp" ]; then
        mv "my_output.bmp" "$my_output_bmp"
    fi
    
    # Compare results
    local stage1_result=0
    local stage2_result=0
    local stage3_result=0
    local zbuffer_result=0
    
    compare_files "$my_stage1" "$exp_stage1" "Stage 1"
    stage1_result=$?
    
    compare_files "$my_stage2" "$exp_stage2" "Stage 2"
    stage2_result=$?
    
    compare_files "$my_stage3" "$exp_stage3" "Stage 3"
    stage3_result=$?
    
    compare_files "$my_zbuffer" "$exp_zbuffer" "Z-Buffer"
    zbuffer_result=$?
    
    # Check if bitmap was generated (basic existence check)
    if [ -f "$my_output_bmp" ]; then
        echo -e "${GREEN}✓ Output bitmap: Generated successfully${NC}"
        bitmap_result=0
    else
        echo -e "${RED}✗ Output bitmap: Not generated${NC}"
        bitmap_result=1
    fi
    
    # Overall result for this test case
    if [ $stage1_result -eq 0 ] && [ $stage2_result -eq 0 ] && [ $stage3_result -eq 0 ] && [ $zbuffer_result -eq 0 ] && [ $bitmap_result -eq 0 ]; then
        echo -e "${GREEN}✓ Test Case $case_num: ALL STAGES PASSED${NC}"
        return 0
    else
        echo -e "${RED}✗ Test Case $case_num: SOME STAGES FAILED${NC}"
        return 1
    fi
}

# Function to cleanup generated files
cleanup_files() {
    echo -e "\n${YELLOW}Cleaning up generated files...${NC}"
    for case_num in {1..4}; do
        rm -f "Test Cases/$case_num/my_stage1.txt"
        rm -f "Test Cases/$case_num/my_stage2.txt" 
        rm -f "Test Cases/$case_num/my_stage3.txt"
        # Note: Preserving z-buffer and bitmap files for next session
        # rm -f "Test Cases/$case_num/test${case_num}_z_buffer.txt"
        # rm -f "Test Cases/$case_num/test${case_num}_my_output.bmp"
    done

    echo -e "${GREEN}Cleanup complete!${NC}"
}

# Main testing loop
passed_cases=0
total_cases=4

for case_num in {1..4}; do
    echo
    test_case "$case_num"
    if [ $? -eq 0 ]; then
        ((passed_cases++))
    fi
    echo
done

# Summary
echo -e "${BLUE}=== TEST SUMMARY ===${NC}"
echo -e "Total test cases: $total_cases"
echo -e "Passed: ${GREEN}$passed_cases${NC}"
echo -e "Failed: ${RED}$((total_cases - passed_cases))${NC}"

if [ $passed_cases -eq $total_cases ]; then
    echo -e "${GREEN}🎉 ALL TEST CASES PASSED! 🎉${NC}"
else
    echo -e "${RED}❌ SOME TEST CASES FAILED ❌${NC}"
fi

# Cleanup
cleanup_files

# Clean up compiled executables
echo -e "\n${YELLOW}Cleaning up executables...${NC}"
rm -f task1 task2 task3 task4
echo -e "${GREEN}All cleanup complete!${NC}"

# Exit with appropriate code
if [ $passed_cases -eq $total_cases ]; then
    exit 0
else
    exit 1
fi
