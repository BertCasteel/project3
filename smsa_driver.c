////////////////////////////////////////////////////////////////////////////////
//
//  File           : smsa_driver.c
//  Description    : This is the driver for the SMSA simulator.
//
//   Authors       : Kevin Beick & Robert Casteel
//   Last Modified : 
//

// Include Files

// Project Include Files
#include <smsa_driver.h>
#include <cmpsc311_log.h>

// Defines

// Functional Prototypes

//
// Global data

// Interfaces

// HELPERS
uint32_t MakeSmsaInstruction(SMSA_DISK_COMMAND opcode, uint32_t DrumID, uint32_t BlockID){
    // Shift DrumID to right position in instruction
    uint32_t ShiftedOpcode = opcode << 26;
    // Shift DrumID to right position in instruction
    uint32_t ShiftedDrum = DrumID << 22;
    // Bitwise OR the codes together
    return (uint32_t)(ShiftedOpcode|ShiftedDrum|BlockID);
}
uint32_t GetDrumID(SMSA_VIRTUAL_ADDRESS addr){
    return addr/SMSA_DISK_SIZE;
}
uint32_t GetBlockID(SMSA_VIRTUAL_ADDRESS addr){
    return (addr%SMSA_DISK_SIZE)/SMSA_BLOCK_SIZE;
}
uint32_t GetOffset(SMSA_VIRTUAL_ADDRESS addr){
    return addr%SMSA_BLOCK_SIZE;
}




////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vmount
// Description  : Mount the SMSA disk array virtual address space
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int smsa_vmount( void ) {
    uint32_t instruction = MakeSmsaInstruction(SMSA_MOUNT,0,0);
    return smsa_operation(instruction,NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vunmount
// Description  :  Unmount the SMSA disk array virtual address space
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int smsa_vunmount( void )  {
    uint32_t instruction = MakeSmsaInstruction(SMSA_UNMOUNT,0,0);
    return smsa_operation(instruction, NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vread
// Description  : Read from the SMSA virtual address space
//
// Inputs       : addr - the address to read from
//                len - the number of bytes to read
//                buf - the place to put the read bytes
// Outputs      : -1 if failure or 0 if successful

int smsa_vread( SMSA_VIRTUAL_ADDRESS addr, uint32_t len, unsigned char *buf ) {
    // Translate Virtual Address to Drum, Block, and offset
    uint32_t curDrum = GetDrumID(addr);
    uint32_t curBlock= GetBlockID(addr);
    uint32_t offset = GetOffset(addr);

    if (len > 
(SMSA_BLOCK_SIZE-offset) + 
(SMSA_MAX_BLOCK_ID - (curBlock + 1)) * SMSA_BLOCK_SIZE +
(SMSA_DISK_ARRAY_SIZE - (curDrum + 1))*(SMSA_MAX_BLOCK_ID * SMSA_BLOCK_SIZE)){
	logMessage( LOG_INFO_LEVEL, "Too large of len");
	return 0;
}	


    // temp buffer used for smsa ops
    unsigned char temp_block_buffer[SMSA_BLOCK_SIZE];

    // Move to the drum to be read
    uint32_t seek_drum_instr = MakeSmsaInstruction(SMSA_SEEK_DRUM, curDrum, curBlock);
    smsa_operation(seek_drum_instr, NULL);

    /* Set up iterator for reading from block; Will start reading from the offset
        (our addr might not start right on block boundary) */
    uint32_t i=offset;
    // counter for how many bytes we've read so far
    uint32_t bytesRead;

    // While we still need to read...
    for (bytesRead = 0; bytesRead < len;)
    {
        // Move to the block to be read
        uint32_t seek_block_instr = MakeSmsaInstruction(SMSA_SEEK_BLOCK, curDrum, curBlock);
        smsa_operation(seek_block_instr, NULL);

        // Read block into temp buffer
        uint32_t read_disk_instr = MakeSmsaInstruction(SMSA_DISK_READ, curDrum, curBlock);
        smsa_operation(read_disk_instr, temp_block_buffer);

        // Copy stuff from temp buffer to real buffer
        for(; i<SMSA_BLOCK_SIZE && bytesRead<len; )
        {
            buf[bytesRead++] = temp_block_buffer[i++];
        }

        // If we've read this entire drum (ie its last block), want to move to next drum.
        // Else just move along to next block.
        if(curBlock==255){
            curDrum++;
            curBlock = 0;
            seek_drum_instr = MakeSmsaInstruction(SMSA_SEEK_DRUM,curDrum,0);
            smsa_operation(seek_drum_instr,NULL);
        }else{ curBlock++; }

        // reset iterator to 0 - we will want all future readings to start at beginning of block
        i = 0;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vwrite
// Description  : Write to the SMSA virtual address space
//
// Inputs       : addr - the address to write to
//                len - the number of bytes to write
//                buf - the place to read the read from to write
// Outputs      : -1 if failure or 0 if successful

int smsa_vwrite( SMSA_VIRTUAL_ADDRESS addr, uint32_t len, unsigned char *buf )  {
// Translate Virtual Address to Drum, Block, and offset
    uint32_t curDrum = GetDrumID(addr);
    uint32_t curBlock= GetBlockID(addr);
    uint32_t offset = GetOffset(addr);

    if (len > 
(SMSA_BLOCK_SIZE-offset) + 
(SMSA_MAX_BLOCK_ID - (curBlock + 1)) * SMSA_BLOCK_SIZE +
(SMSA_DISK_ARRAY_SIZE - (curDrum + 1))*(SMSA_MAX_BLOCK_ID * SMSA_BLOCK_SIZE)){
	logMessage( LOG_INFO_LEVEL, "Too large of len");
	return 0;
}	

    // temp buffer used for smsa ops
    unsigned char temp_block_buffer[SMSA_BLOCK_SIZE];

    // Move to the drum to be read
    uint32_t seek_drum_instr = MakeSmsaInstruction(SMSA_SEEK_DRUM, curDrum, curBlock);
    smsa_operation(seek_drum_instr, NULL);

    /* Set up iterator for reading from block; Will start reading from the offset
        (our addr might not start right on block boundary) */
    uint32_t i=offset;
    // counter for how many bytes we've read so far
    uint32_t bytesWritten;

    // While we still need to read...
    for (bytesWritten = 0; bytesWritten < len;)
    {
		// First, seek the appropriate block and read it into our temp. This will be our working copy of the block/
		
        // Move to the block to be read
        uint32_t seek_block_instr = MakeSmsaInstruction(SMSA_SEEK_BLOCK, curDrum, curBlock);
        smsa_operation(seek_block_instr, NULL);

        // Read block into temp buffer - head will now point to next block
        uint32_t read_disk_instr = MakeSmsaInstruction(SMSA_DISK_READ, curDrum, curBlock);
        smsa_operation(read_disk_instr, temp_block_buffer);


        // Copy stuff from input buffer to temp buffer (if not manipulating entire block, unmanipulated elements reflect what was originally in block)
        for(; i<SMSA_BLOCK_SIZE && bytesWritten<len;)
        {
            temp_block_buffer[i++] = buf[bytesWritten++];
        }

        // Move to the current block again (reads and writes leave head at subsequent block; essentially we wanna move back a block)
        seek_block_instr = MakeSmsaInstruction(SMSA_SEEK_BLOCK, curDrum, curBlock);
        smsa_operation(seek_block_instr, NULL);

        // write to block from temp buffer
        uint32_t write_disk_instr = MakeSmsaInstruction(SMSA_DISK_WRITE, curDrum, curBlock);
        smsa_operation(write_disk_instr, temp_block_buffer);


        // If we've read this entire drum (ie its last block), want to move to next drum.
        // Else just move along to next block.
        if(curBlock==255){
            curDrum++;
            curBlock = 0;
            seek_drum_instr = MakeSmsaInstruction(SMSA_SEEK_DRUM,curDrum,0);
            smsa_operation(seek_drum_instr,NULL);
        }else{ curBlock++; }

        // reset iterator to 0 - we will want all future readings to start at beginning of block
        i = 0;
    }
    return 0;
}
