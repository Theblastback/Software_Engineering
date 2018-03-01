# Build sample program that uses BGIFONT.TPU and BGIDRIV.TPU
#t-robots.exe: bgidriv.tpu trobfont.tpu
#tpc t-robots /m /l


egavga.tpu: egavga.pas egavga.obj
  tpc egavga
egavga.obj: egavga.bgi
  binobj egavga.bgi egavga EGADriverProc
