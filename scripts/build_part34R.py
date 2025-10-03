# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
#!/usr/bin/env python3
import argparse, os, pathlib, zipfile, json, shutil
TODAY="2025-10-02"
def sz(p): return p.stat().st_size
def main():
  ap=argparse.ArgumentParser()
  ap.add_argument("--repo-path", required=True)
  ap.add_argument("--target-size-mb", type=int, default=230)
  ap.add_argument("--hard-cap-mb", type=int, default=300)
  ap.add_argument("--out", default=".")
  ap.add_argument("--force-refactor", action="store_true")
  a=ap.parse_args()
  repo=pathlib.Path(a.repo_path).resolve()
  out=pathlib.Path(a.out).resolve(); out.mkdir(parents=True, exist_ok=True)
  kit_root=pathlib.Path(__file__).resolve().parent.parent
  wanted=["templates/","docs/","tools/","tasks/","include/","src/plugins/transpile/","scripts/"]
  stage=out/"stage_part34R"; 
  if stage.exists(): shutil.rmtree(stage)
  stage.mkdir(parents=True)
  def want(rel): 
    s=rel.as_posix()
    return any(s.startswith(p) for p in wanted)
  manifest={"created":TODAY,"files":[]}; changelog=[]
  # copy repo first
  for p in repo.rglob("*"):
    if p.is_file():
      rel=p.relative_to(repo)
      if want(rel):
        dest=stage/rel; dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(p,dest)
        manifest["files"].append({"src":"repo","rel":rel.as_posix(),"bytes":sz(dest)})
  # overlay kit (no-downsize)
  for p in kit_root.rglob("*"):
    if not p.is_file() or p.suffix==".zip": continue
    rel=p.relative_to(kit_root)
    if not want(rel): continue
    dest=stage/rel; dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.exists():
      if sz(p)>=sz(dest) or a.force_refactor:
        shutil.copy2(p,dest); changelog.append({"action":"REPLACED","rel":rel.as_posix()})
      else:
        changelog.append({"action":"KEPT_REPO","rel":rel.as_posix()})
    else:
      shutil.copy2(p,dest); changelog.append({"action":"ADDED","rel":rel.as_posix()})
  # chunk into zips
  soft=a.target_size_mb*1024*1024; hard=a.hard_cap_mb*1024*1024
  idx=1; cur=None; cur_sz=0
  def open_zip():
    nonlocal cur,cur_sz,idx
    if cur: cur.close()
    cur=zipfile.ZipFile(out/f"umicom_studio_part34R_{idx:02d}.zip","w",zipfile.ZIP_DEFLATED)
    cur_sz=0; idx+=1
  open_zip()
  for p in sorted(stage.rglob("*")):
    if not p.is_file(): continue
    rel=str(p.relative_to(stage))
    if cur_sz + sz(p) > hard:
      open_zip()
    cur.write(p, arcname=rel); cur_sz += sz(p)
  if cur: cur.close()
  with open(out/"CHANGELOG_PART34R.json","w",encoding="utf-8") as f: json.dump(changelog,f,indent=2)
  with open(out/"MANIFEST_PART34R.json","w",encoding="utf-8") as f: json.dump(manifest,f,indent=2)
  print("OK — Parts written to", str(out))
if __name__=="__main__": main()
