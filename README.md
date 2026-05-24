City Infrastructure Project-second year project 

This project implements a management system for city infrastructure reports. Each district is represented by a separate directory that contains the files needed for storing reports, configuring the severity threshold, and recording performed actions.

The main program allows the user to add, list, view, filter, and remove reports. Reports are stored in a binary file called `reports.dat`, using fixed-size structures. Each district also contains a configuration file called `district.cfg`, where the severity threshold is stored, and a log file called `logged_district`, where important actions are recorded.

The project uses Unix file and directory permissions. Permissions are set with `chmod()` and checked with `stat()`. For each district, the program also creates a symbolic link named `active_reports-<district>`, which points to the corresponding `reports.dat` file.

The project also includes a monitoring program called `monitor_reports`. This program creates a hidden `.monitor_pid` file, responds to `SIGUSR1` when a new report is added, and stops when it receives `SIGINT`.

The final part of the project includes `city_hub`, an interactive command-line interface. It can start and stop the monitor and calculate inspector workload scores for multiple districts. This part uses child processes, pipes, `dup2()`, and external programs. The external program `scorer.c` calculates the sum of severity levels for each inspector.

Main files:
- `main.c` – main program for managing districts and reports
- `monitor_reports.c` – monitoring program
- `city_hub.c` – interactive hub program
- `scorer.c` – external program for calculating inspector workload scores
- `AI_usage-ALL-phases.md` – description of AI usage during the project
- `commands.txt` – useful commands for compiling and testing the project
