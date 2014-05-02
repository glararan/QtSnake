#ifndef OPCODES_H
#define OPCODES_H

enum Opcodes
{
    // Hi
    Hello    = 0x00,

    // Snake Broadcast
    BroadcastSnake = 0x10,

    // Snake
    SnakeCorrection = 0x11,

    // Player
    PlayerAdd    = 0x20,
    PlayerRemove = 0x21,

    // G.O.
    GameOver = 0xff
};

#endif // OPCODES_H