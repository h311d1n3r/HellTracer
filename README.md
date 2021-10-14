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
`mem` -> Turns on tracing of the specified memory range. e.g: 'mem ascii=[[rbp-0x40]:4]', 'mem ascii=[@0x40200f:15]', 'mem [rsi]'  
`args` -> Specifies the arguments to be passed to the traced binary. e.g: 'args "./name.bin hello world"'  
`output` -> Specifies the output file (.csv). e.g: 'output out.csv'
### Flags
`help` -> Displays the help message.  
`reg_name` -> Turns on tracing of register reg_name. e.g: 'rip', 'rcx', 'rsp', 'all'
### Example
#### Command
The following command will write to ./out.csv, the content of the rip register in hexadecimal and of memory region starting at 0x40200F and of size 11 in ASCII and memory region starting at RSI of size 5 in ASCII, during the execution of the binary ./example.bin with program arguments "./example.bin username password" when rip is contained between 0x400000 and 0x500000, with entry point defined at 0x4011a0.  
`sudo helltracer ./example.bin -output ./out.csv -args "./example.bin username password" -entry 0x4011a0 -start 0x400000 -end 0x500000 --rip -mem ascii=[@0x40200f:11] -mem ascii=[rsi:5]`
#### Result
Here is the beginning of the resulting CSV file :  
  
<p align="center">
  <img src="https://i.imgur.com/fKxb5Rn.png" alt="result.png"/>
</p>
