import math, struct, zlib, pathlib
OUT = pathlib.Path(__file__).resolve().parent.parent / "web" / "icons"
OUT.mkdir(parents=True, exist_ok=True)

def png(size):
    pixels = bytearray(size * size * 4)
    def put(x,y,c):
        if 0 <= x < size and 0 <= y < size:
            i=(y*size+x)*4
            pixels[i:i+4]=bytes((*c,255))
    def line(x1,y1,x2,y2,width,color):
        steps=max(int(math.ceil(max(abs(x2-x1),abs(y2-y1)))),1)
        r=width/2
        for n in range(steps+1):
            x=x1+(x2-x1)*n/steps
            y=y1+(y2-y1)*n/steps
            for yy in range(int(y-r-1),int(y+r+2)):
                for xx in range(int(x-r-1),int(x+r+2)):
                    if (xx-x)**2+(yy-y)**2 <= r*r: put(xx,yy,color)
    cx=cy=size/2
    scale=size/512
    for y in range(size):
        for x in range(size):
            t=(x+y)/(2*size)
            put(x,y,(int(15+8*t),int(23+13*t),int(35+20*t)))
    # Gold circular crest
    radius=210*scale
    ring=13*scale
    for y in range(size):
        for x in range(size):
            d=math.hypot(x-cx,y-cy)
            if abs(d-radius)<ring/2:
                glow=max(0,1-abs(d-radius)/(ring/2))
                put(x,y,(int(174+60*glow),int(119+55*glow),int(61+45*glow)))
    # Crossed blade / tag emblem
    gold=(218,172,104); silver=(231,224,207); dark=(75,53,40)
    line(0.40*size,0.68*size,0.68*size,0.34*size,25*scale,silver)
    line(0.34*size,0.70*size,0.70*size,0.34*size,8*scale,gold)
    line(0.37*size,0.52*size,0.54*size,0.66*size,22*scale,gold)
    line(0.34*size,0.55*size,0.49*size,0.70*size,7*scale,dark)
    line(0.61*size,0.30*size,0.72*size,0.41*size,20*scale,gold)
    # Simple central T
    line(0.40*size,0.40*size,0.60*size,0.40*size,17*scale,gold)
    line(0.50*size,0.40*size,0.50*size,0.59*size,17*scale,gold)
    raw=b"".join(b"\x00"+bytes(pixels[y*size*4:(y+1)*size*4]) for y in range(size))
    def chunk(kind,data):
        return struct.pack(">I",len(data))+kind+data+struct.pack(">I",zlib.crc32(kind+data)&0xffffffff)
    return b"\x89PNG\r\n\x1a\n"+chunk(b"IHDR",struct.pack(">IIBBBBB",size,size,8,6,0,0,0))+chunk(b"IDAT",zlib.compress(raw,9))+chunk(b"IEND",b"")

for s in (192,512):
    (OUT/f"icon-{s}.png").write_bytes(png(s))
print("Generated", *(str(OUT/f"icon-{s}.png") for s in (192,512)))
