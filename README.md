## Page Rank

### Compile 
To compile the code, run the following command:

        $ make

### Run
To run the code, run the following command:
        
        $ ./page_rank <input_file> <number_of_threads> (-i <number_of_iterations>) (-m <mode>)
        
        <mode> argument: 0 is the default option, program produces csv file with resutls
                         other value doesn't produce files, used for speed measurements

### Input format
The input file should be in the following format:

                # Comments start with a hash        
                <source> <destination>
                <source> <destination>
                ...
                <source> <destination>

### Speed Measurements
To test the speed of the code and see difference between the amount of threads,
as well as the average and standard deviation and average speedup, run runner.py file with the following command:

        $ python my_script.py --num_runs 10 --num_threads 8 --exec_name my_executable --test_folder_path my_tests --test_sets test1.txt test2.txt

Results are then saved in the results.json file.
