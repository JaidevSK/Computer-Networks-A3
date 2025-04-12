# Computer-Networks-A3

## Directions to Run the Code

### Q1
- For a, run the following command in terminal:
```
sudo python3 Q1a.py
``` 

- For b, first unzip pox in the same location and in one of the terminals, run
```
cd pox
./pox.py openflow.discovery openflow.spanning_tree forwarding.l2_learning stp-config
```
- in the other terminal, run 
```
sudo python3 Q1b.py
```

### Q2
- First run
```
cd pox
./pox.py openflow.discovery openflow.spanning_tree forwarding.l2_learning stp-config
```
- To run the basic variant, execute 
```
sudo python3 Q2.py
```
- Torun the variant with switches, execute
```
sudo python3 Q2_.py
```

### Q3
- To compile, use any compiler which compiles all the C files into an executable. To use MSVC, the below command can be used.
```
cl /W4 /Fe:routing.exe node0.c node1.c node2.c node3.c distance_vectore.c
```
- To execute, run
```
routing.exe
```
To enable debugging, give TRACE=2 when prompted.

