import re, glob, os
names=[l.split(';')[0].strip() for l in open('/root/dtu-build/pristine/DTU.MSN') if '.lvl' in l]
rows=[]
for n in range(33):
    d='/root/dtu-build/lev%d'%n; f=d+'/dtu.log'
    if not os.path.exists(f): rows.append((n,names[n],'(niet gedraaid)')); continue
    L=open(f,errors='replace').read()
    ex=re.search(r'exit code (\d+) after (\d+) game frames',L)
    crash='CRASH' if re.search(r'exception|crash|fault at|signal',L,re.I) and 'exit code' not in L else ''
    fr=re.findall(r'\[frame\] (\d+) frames, frame time min ([\d.]+) avg ([\d.]+) max ([\d.]+) ms, (\d+) out of range; game work avg ([\d.]+) max ([\d.]+)',L)
    work=max((float(x[5]) for x in fr),default=0); wmax=max((float(x[6]) for x in fr),default=0)
    ftmax=max((float(x[3]) for x in fr),default=0)
    lowm=re.search(r'(\d+) low-memory accesses',L)
    nf=sorted(set(x for x in re.findall(r'open D:\\DTU\\(\S+) \(mode 0\) -> not found',L)))
    nf=[x for x in nf if not re.search(r'(?i)\.mve|\.hk1|sg\d|\.roc|hmc\.|\.egg|\.bin|\.ls|\.lvl|\.dog|\.end|\.txb|fnt|sg[0-9]_',x)]
    err=re.findall(r'\[dos\] stdout: \n?(Error[^\n]*)',L)
    i3=len(re.findall(r'breakpoint',L))
    rows.append((n,names[n], 'exit=%s frames=%s work_avg=%.1fms work_max=%.0fms ft_max=%.0f lowmem=%s int3=%d %s %s %s'%(
        ex.group(1) if ex else '?', ex.group(2) if ex else '?', work, wmax, ftmax, lowm.group(1) if lowm else '?', i3, crash, ' '.join(err)[:80], ('missing:'+','.join(nf[:4])) if nf else '')))
for r in rows: print('%2d %-13s %s'%r)
