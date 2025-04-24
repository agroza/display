; ---------------------------------------------------------------------------
; - DISPLAY - Text-Mode File Viewer Program (display.asm)                   -
; - Copyright (C) 1998-2025 Alexandru Groza of Microprogramming TECHNIQUES  -
; - All rights reserved.                                                    -
; ---------------------------------------------------------------------------
; - License: GNU General Public License v3.0                                -
; ---------------------------------------------------------------------------

LOCALS @@

P8086
MODEL TINY

CODESEG
  org 100h

STARTUPCODE

  jmp @start

; include section
; ---------------------------------------------------------------------------
include equates.inc
include strings.inc
include vars.inc
include program.inc
include routines.inc
include video.inc

; Main program entrypoint.
; ---------------------------------------------------------------------------
@start:
  call fileOpenLoadClose
  jc @displayError

@continue:
  call videoDisableCursor

  call programWriteHeader
  call programMainLoop

  call videoEnableCursor

  mov dh, kVGARowCount - 1
  xor dl, dl
  call videoMoveCursor

  mov es, word ptr [GSegmentAllocated]
  mov ah, 49h
  int 21h

@exit:
  mov ax, 4C00h
  int 21h

@displayError:
  call videoWriteln
  jmp short @exit

programEnd:

end
