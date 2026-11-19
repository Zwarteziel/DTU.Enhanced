# Draws the program icon (dtu.ico, 16-256 pixels) for dtu.rc: a mountain with a
# lit archway and steps leading down into it. Usage: python3 make_icon.py
from PIL import Image, ImageDraw, ImageFilter, ImageChops
import math, os
HERE = os.path.dirname(os.path.abspath(__file__))
S=1024
def lerp(a,b,t): return tuple(int(a[i]+(b[i]-a[i])*t) for i in range(len(a)))
def mountain(S):
    img=Image.new('RGBA',(S,S),(0,0,0,0))
    # mountain silhouette with vertical gradient
    pts=[(0.03,0.93),(0.20,0.52),(0.30,0.60),(0.50,0.10),(0.62,0.30),(0.70,0.24),(0.97,0.93)]
    P=[(x*S,y*S) for x,y in pts]
    mask=Image.new('L',(S,S),0); ImageDraw.Draw(mask).polygon(P,fill=255)
    grad=Image.new('RGBA',(S,S))
    gd=ImageDraw.Draw(grad)
    for y in range(S):
        t=y/S; gd.line([(0,y),(S,y)],fill=lerp((126,136,158,255),(38,40,52,255),min(1,max(0,(t-0.1)/0.85))))
    img.paste(grad,(0,0),mask)
    d=ImageDraw.Draw(img)
    # snow cap
    snow=[(0.50*S,0.10*S),(0.585*S,0.25*S),(0.55*S,0.23*S),(0.52*S,0.28*S),(0.48*S,0.22*S),(0.44*S,0.25*S),(0.415*S,0.245*S)]
    d.polygon(snow,fill=(225,230,240,255))
    # ridge shading: right face darker
    shade=Image.new('L',(S,S),0)
    ImageDraw.Draw(shade).polygon([(0.50*S,0.10*S),(0.62*S,0.30*S),(0.70*S,0.24*S),(0.97*S,0.93*S),(0.50*S,0.93*S),(0.52*S,0.5*S)],fill=90)
    shade=ImageChops.multiply(shade,mask)
    img=Image.composite(Image.new('RGBA',(S,S),(10,10,20,255)),img,shade)
    return img,mask
img,mask=mountain(S)
d=ImageDraw.Draw(img)
# glow behind doorway
cx,base=0.5*S,0.93*S; w=0.30*S; h=0.40*S
glow=Image.new('RGBA',(S,S),(0,0,0,0))
gd=ImageDraw.Draw(glow)
for r in range(10,0,-1):
    k=r/10
    gd.ellipse([cx-w*1.15*k,base-h*0.95-w*0.6*k,cx+w*1.15*k,base-h*0.15+w*0.6*k],fill=(255,150,40,int(70*(1-k)+8)))
glow=glow.filter(ImageFilter.GaussianBlur(S*0.03))
glow_m=Image.composite(glow,Image.new('RGBA',(S,S),(0,0,0,0)),mask)
img=Image.alpha_composite(img,glow_m)
d=ImageDraw.Draw(img)
# stone frame of the arch
fw=w*1.30; fh=h*1.10
def arch(d,cx,base,w,h,fill):
    d.rectangle([cx-w/2,base-h+w/2,cx+w/2,base],fill=fill)
    d.ellipse([cx-w/2,base-h,cx+w/2,base-h+w],fill=fill)
arch(d,cx,base,fw,fh,(70,64,60,255))
# stones lines on the frame
for i in range(7):
    a=math.pi*(i+0.5)/7
    r1=w/2; r2=fw/2; ccy=base-fh+fw/2
    ox=cx-math.cos(a)*r1; oy=ccy-math.sin(a)*r1*(fw/fw)
    px=cx-math.cos(a)*r2; py=(base-fh+fw/2)-math.sin(a)*r2
    d.line([(cx-math.cos(a)*r1,(base-h+w/2)-math.sin(a)*r1),(px,py)],fill=(40,36,34,255),width=int(S*0.008))
# glowing doorway
inner=Image.new('RGBA',(S,S),(0,0,0,0)); idr=ImageDraw.Draw(inner)
arch(idr,cx,base,w,h,(255,255,255,255))
imask=inner.split()[3]
door=Image.new('RGBA',(S,S))
dd=ImageDraw.Draw(door)
for y in range(S):
    t=(y-(base-h))/h; t=min(1,max(0,t))
    dd.line([(0,y),(S,y)],fill=lerp((255,224,140,255),(200,80,20,255),t))
img.paste(door,(0,0),imask)
d=ImageDraw.Draw(img)
# stairs descending into the doorway (dark steps, narrowing)
steps=5
for i in range(steps):
    t0=i/steps; t1=(i+1)/steps
    y0=base-0.02*S-(t0)*h*0.42; y1=base-0.02*S-(t1)*h*0.42
    ww0=w*(1-0.55*t0); ww1=w*(1-0.55*t1)
    col=lerp((30,22,20,255),(120,60,25,255),t0)
    d.polygon([(cx-ww0/2,y0),(cx+ww0/2,y0),(cx+ww1/2,y1),(cx-ww1/2,y1)],fill=col)
    d.line([(cx-ww1/2,y1),(cx+ww1/2,y1)],fill=(255,190,90,255),width=max(2,int(S*0.006)))
# ground line
d.rectangle([0.03*S,0.93*S-2,0.97*S,0.95*S],fill=(30,30,38,255))
sizes=[256,128,64,48,32,24,16]
ims=[img.resize((s,s),Image.LANCZOS) for s in sizes]
ims[0].save(os.path.join(HERE, 'dtu.ico'),sizes=[(s,s) for s in sizes],append_images=ims[1:])
