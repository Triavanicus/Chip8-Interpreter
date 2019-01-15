# Chip8-Interpreter
Just a simple chip8 Interpreter, with my own little twist.

## Twist you say?
So I decided to go with pointers for I SP and PC (image, stack pointer, program counter) instead of the normal convention.
My only sadness was that I couldn't do it fully the way I wanted to (stack pointer pointing to the stack, which holds pointers)
but I made a constraint on myself to limit the data inside of the 512 byte mark, inline with the memory.

## Something nobody else has done (I think)
So in all of the source code I used to review my own, I noticed that everyone loads the font into the memory, but then
does something like `case Fx29: I = 5 * ((opcode & 0x0f00) >> 8); break;` when they have to load 0-F from the font
in the memory, where as in my solution I have a union with a memory array, and a struct with all of my interpreter variables
including the font, which I had the font as a multidimensional array for characters 0-F bytes 1-5. This allowed me to have the
simpler solution of `case Fx29: I = font[(opcode & 0x0f00) >> 8]; break;`

## Constraints
So as I mentioned previously I had to put all of the interpreter variables inside of the first 512 bytes of the interpreter's
`memory`. Something most people do is they have an array of `pixels`. The size of the pixel array is 64x32 because that is the number
of pixels Chip8 uses. However when you do that, you only use one bit out of every pixel. Also doing that was going to blow my
bits budget because I needed to have those lovely, raw, 64-bit pointer. So what I did instead was use every single bit.
Yep... like the good old times. So with doing this I discovered something. It is really easy to get the collision detection
functioning. So the way collision works with Chip8 is when you draw to the screen you xor the sprite into the bits. If a bit flips
from on to off, it sets a flag at register `Vf` to 1. So in order to get from the x/y coordinates to xy bit coordinates you have to
take `x / 8`. But now you have to worry about if a sprite isn't getting drawn to an even byte, so you have to keep
track of how much you have to shift by, which is simple because it is just `x % 8`. So back to the collision, how is this easy,
well if you take `pixel[pixelLocation]` and, well, and it with the sprite shifted by the shift amount, if there was no collision,
it would be 0, because there weren't any colliding bits. `if((pixel[pixelLocation] & (sprite <<>> shiftAmount)) != 0)` So that
took out a for loop and I saved a total of 5 << >> or & per 8 pixels. So the first bitshift is to the left `<<` by `x % 8` then
the second one for the adjacent pixel is to the right `>>` by `8 - (x % 8)`

## Conclusion
Just becuase you see everyone else writing something one way, challenge yourself to find another way. You never know just what
you are going to find.
