## Page Rank

To compile the code, run the following command:

        $ make

To run the code, run the following command:
        
        $ ./page_rank <input_file> <number_of_threads> (-i <number_of_iterations>) (-m <mode>)
        
        <mode> argument: 0 is the default option, program produces csv file with resutls
                         other value doesn't produce files, used for speed measurements

The input file should be in the following format:

                # Comments start with a hash        
                <source> <destination>
                <source> <destination>
                ...
                <source> <destination>
