I found the way to separate numbers so that 100 millions look like 

`100'000'000`

... instead of ...

`100000000`

But the compiling command needs the following addition:
```sh
--std=c2x
```

So the final command is:
```sh
gcc -fopenmp --std=c2x task1.c -o task1
```

Part 2 tasks need the addition
