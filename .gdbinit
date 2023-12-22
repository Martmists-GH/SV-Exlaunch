# This file contains useful utilities for handling GDB with this project.

set  disassemble-next-line on
show disassemble-next-line

# Dump an EXL_ABORT:
# Use only when you see str x28, [x27]
define switch_abort
  printf "Result     : %ld\n", $x21
  printf "File       : %s\n", $x22
  printf "Line       : %d\n", $x23
  printf "Function   : %s\n", $x24
  printf "Expression : %s\n", $x25
  printf "Message    : %s\n", $x26
end

# Set this to the relative base of the module you want to debug; Can be done through GDB console.
set $base_addr = 0
# Offset used by Ghidra; Change if not using the default from ghidra-switch-loader
set $ghidra_offset_base = 0x7100000000

# Dump a backtrace of the current thread.
define switch_bt
  set $frame = $fp
  set $prev_frame = 0
  while $frame != 0 && $prev_frame != $frame
      set $prev_frame = $frame
      set $frame_addr = ((unsigned long long *)$frame)[1] - $base_addr

      # Print the address this would have in ghidra
      p/x $frame_addr - $base_addr - 4 + $ghidra_offset_base
      # Dump the ASM with 1 instruction before and after
      x/3i $frame_addr - 8

      set $frame = ((unsigned long long *)$frame)[0]
  end
end
