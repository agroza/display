; ---------------------------------------------------------------------------
; - DISPLAY - String Constants (strings.inc)                                -
; - Copyright (C) 1998-2025 Alexandru Groza of Microprogramming TECHNIQUES  -
; - All rights reserved.                                                    -
; ---------------------------------------------------------------------------
; - License: GNU General Public License v3.0                                -
; ---------------------------------------------------------------------------

; program stringtable
; ---------------------------------------------------------------------------
  sProgram           DB 'DISPLAY 1.2 (C) 2025 Microprogramming TECHNIQUES   //PgUp/PgDn=Scroll ESC=Exit',0

  sParameterError    DB 'Use: display.com <file.ext>',0
  sFileError         DB 'Error reading file.',0
  sMemoryError       DB 'Not enough memory.',0
