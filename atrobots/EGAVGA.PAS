{ Portions Copyright (c) 1985, 1990 by Borland International, Inc. }
(*
Copyright (c) 1999, Ed T. Toton III. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   All advertising materials mentioning features or use of this software
   must display the following acknowledgement:

        This product includes software developed by Ed T. Toton III &
        NecroBones Enterprises.

   No modified or derivative copies or software may be distributed in the
   guise of official or original releases/versions of this software. Such
   works must contain acknowledgement that it is modified from the original.

   Neither the name of the author nor the name of the business or
   contributers may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*)
{$G+}{$N+}{$E+}{$X+}{$D-}
unit egavga;
{ This unit links the EGA/VGA BGI graphics driver into a single TPU file.
  This makes it easy to link the driver files directly into an .EXE file. }
interface

uses graph;


procedure egavgaProc;
procedure graph_Abort(Msg : string);
Procedure Graph_VGA;

implementation


procedure egavgaProc; external;
{$L egavga.OBJ }

procedure graph_Abort(Msg : string);
begin
  Writeln(Msg, ': ', GraphErrorMsg(GraphResult));
  Halt(1);
end;

Procedure Graph_VGA;
var
 driver, mode:integer;
 i:integer;
begin
   if RegisterBGIdriver(@EGAVGAProc) < 0 then
     graph_Abort('EGA/VGA');

      driver:=VGA; mode:=VGAHI;
      initgraph(driver,mode,'');
      i:=graphresult;
      if (i<>0) then
       begin
        writeln('Graphics Error: ',GraphErrorMsg(i));
	writeln('You must have a VGA to use 640x480x16 mode');
	halt;
       end;
end;

end.
