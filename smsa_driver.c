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


uint32_t MakeSmsaInstruction(SMSA_DISK_COMMAND opcode, uint32_t DrumID, uint32_t BlockID){
    // Shift DrumID to right position in instruction
    uint32_t ShiftedOpcode = opcode << 26;
    // Shift DrumID to right position in instruction
    uint32_t ShiftedDrum = DrumID << 22;
    // Bitwise OR the codes together
    return (uint32_t)(ShiftedOpcode|ShiftedDrum|BlockID);
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

}
