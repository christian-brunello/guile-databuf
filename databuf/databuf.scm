;
; databuf.scm
; This file is part of databuf
;
; Copyright (C) 2018 Christian Brunello
;
;  This program is free software; you can redistribute it and/or modify
;  it under the terms of the GNU Library General Public License as published by
;  the Free Software Foundation; either version 2 of the License, or
;  (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU Library General Public License for more details.
;
;  You should have received a copy of the GNU Library General Public License
;  along with this program; if not, see <http://www.gnu.org/licenses/>.
;

(define-module (databuf)
 #:export (
  DATABUF_ENDIANNESS_BIG
  DATABUF_ENDIANNESS_LITTLE
  make-databuf
  databuf-seek!
  databuf-write-u8!
  databuf-write-s8!
  databuf-write-u16!
  databuf-write-s16!
  databuf-write-u32!
  databuf-write-s32!
  databuf-write-u64!
  databuf-write-s64!
  databuf-write-double!
  databuf-write-string!
  databuf-read-u8
  databuf-read-s8
  databuf-read-u16
  databuf-read-s16
  databuf-read-u32
  databuf-read-s32
  databuf-read-u64
  databuf-read-s64
  databuf-read-double
  databuf-read-string
  databuf->bytevector
  bytevector->databuf))

(load-extension "guile-databuf" "init_guile_databuf")


