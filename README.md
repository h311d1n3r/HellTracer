# HellTracer
## Description
A Linux x64 tool to trace registers and memory regions.
## Build the tool
1. Clone the repository.
2. Compile the tool with `make`.
3. Add the generated `bin` directory to your path using `PATH=$PATH:~/path/to/the/repo/bin`.
## How to use ?
### Syntax
`helltracer binary [-param value] [--flag]`
### Parameters
`entry` -> Specifies the program entry point. e.g: 'entry 0x401000'  
`start` -> Specifies the program start address for tracing. e.g: 'entry 0x4010f0'  
`end` -> Specifies the program end address for tracing. e.g: 'entry 0x40115e'  
`mem` -> Turns on tracing of the specified memory range. e.g: 'mem 0x7f5310:0x7f531e', 'mem rsp+4:rsp+12'  
`args` -> Specifies the arguments to be passed to the traced binary. e.g: 'args "./name.bin hello world"'  
`output` -> Specifies the output file (.csv). e.g: 'output out.csv'
### Flags
`help` -> Displays the help message.  
`reg_name` -> Turns on tracing of register reg_name. e.g: 'rip', 'rcx', 'rsp', 'all'
### Example
#### Command
The following command will write to ./out.csv, the content of the rcx and rip registers and memory regions from 0x400150 to 0x40015c and from rip+0 to rip+0x2 during the execution of the binary ./example.bin with program arguments "./example.bin username password" when rip is contained between 0x400000 and 0x500000.  
`sudo helltracer ./example.bin -output ./out.csv -args "./example.bin username password" -entry 0x400000 -start 0x400000 -end 0x500000 --rip --rcx -mem rsp+0:rsp+4 -mem rip+0:rip+0x2`
#### Result
Here is the beginning of the resulting CSV file :  
  
![result.png](https://i.imgur.com/dU6iVj2.png)
