## How to compile
In `HW3/src/`, run:
```
$ make clean && make
```
It will generate the executable file `hw3` in `HW3/bin/`.

## How to run
Usage: `./hw3 <hardblocks> <nets> <pl> <floorplan> <dead_space_ratio> optional(<isThread>)`

example 1(multi-thread):
```
$ ./hw3 ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100.1.floorplan 0.1
```

example 2(singal thread):
```
$ ./hw3 ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100.1.floorplan 0.1 0
```