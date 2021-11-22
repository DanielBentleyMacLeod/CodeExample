// returns the next available block and marks it as used                                                            
int get_available_block(int type) {
  char buffer[BLOCK_SIZE];
  int num_byte;
  int final_byte;
  int num_bit;
  char empty[BLOCK_SIZE] = {0};
  int block_num;

  // read the bitmap block into the buffer                                                                          
  block_read(BITMAP_START, buffer);

  // if we are looking for a free inode block                                                                       
  if (type == INODE_BLOCK) {
    num_byte = INODE_BITMAP_START;
    final_byte = DATA_BITMAP_START;
  }
  // if we are looking for a free data block                                                                        
  else if (type == DATA_BLOCK) {
    num_byte = DATA_BITMAP_START;
    final_byte = BITMAP_SIZE + 1;
  }
  // invalid type (for debugging)                                                                                   
  else {
    HALT("Invalid type parameter to get_available_block");
  }

  // iterate until we find a byte with a free bit or until data section                                             
  while ((int) buffer[num_byte] == 255 && num_byte != final_byte) {
    num_byte++;
  }
  // if we ended up finding no free block                                                                           
  if (num_byte == final_byte) {
    return -1;
  }
  // go through each bit in that byte until we find the zero-bit                                                    
  num_bit = 0;
  while (read_bit(buffer, num_byte * 8 + num_bit) == 1) {
    num_bit++;
  }

  // return the number of the block                                                                                 
  if (type == INODE_BLOCK) {
    // update the bit in the buffer and write buffer back as bitmap                                                 
    write_bit(buffer, num_byte * 8 + num_bit, 1);
    block_write(BITMAP_START, buffer);
    return (num_byte * 8 + num_bit);
  }
  else {
    block_num = DATA_START + ((num_byte - DATA_BITMAP_START) * 8 + num_bit);
    if (block_num > 2047) {
      return -1;
    }
    // update the bit in the buffer and write buffer back as bitmap                                                 
    write_bit(buffer, num_byte * 8 + num_bit, 1);
    block_write(BITMAP_START, buffer);

    // zero out the block                                                                                           
    block_write(block_num, empty);
    return block_num;
  }
  return -1;
}
