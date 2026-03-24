#!/usr/bin/env python3
import argparse
import os
import ROOT

DEFAULT_OUTDIR = "/eos/home-j/jusong/www/iRPC/3024"


def walk_directory(root_dir, prefix=""):
    for key in root_dir.GetListOfKeys():
        name = key.GetName()
        obj = key.ReadObj()
        full_name = f"{prefix}/{name}" if prefix else name
        if obj.InheritsFrom("TDirectory"):
            yield from walk_directory(obj, full_name)
        else:
            yield full_name, obj


def sanitize_filename(name):
    return name.replace("/", "__")


def main():
    parser = argparse.ArgumentParser(description="Export rechit histograms to PNG.")
    parser.add_argument("--input", default="rechit_hist.root", help="Input ROOT file")
    parser.add_argument("--outdir", default=DEFAULT_OUTDIR, help="Output PNG directory")
    parser.add_argument("--size", type=int, default=900, help="Base canvas size in pixels")
    args = parser.parse_args()

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(1110)

    in_path = os.path.abspath(args.input)
    if not os.path.exists(in_path):
        raise FileNotFoundError(f"Input file not found: {in_path}")

    os.makedirs(args.outdir, exist_ok=True)
    f = ROOT.TFile.Open(in_path)
    if not f or f.IsZombie():
        raise RuntimeError(f"Failed to open ROOT file: {in_path}")

    n_saved = 0
    n_skipped = 0
    for full_name, obj in walk_directory(f):
        is_h1 = obj.InheritsFrom("TH1") and not obj.InheritsFrom("TH2")
        is_h2 = obj.InheritsFrom("TH2")
        if is_h1:
            draw_opt = "hist"
            c = ROOT.TCanvas("c_" + sanitize_filename(full_name), "", args.size, args.size)
            c.SetMargin(0.12, 0.05, 0.12, 0.08)
        elif is_h2:
            draw_opt = "colz"
            left, right, bottom, top = 0.12, 0.16, 0.12, 0.08
            frame_h = args.size
            canvas_h = int(round(frame_h / (1.0 - top - bottom)))
            canvas_w = int(round(frame_h / (1.0 - left - right)))
            c = ROOT.TCanvas("c_" + sanitize_filename(full_name), "", canvas_w, canvas_h)
            c.SetMargin(left, right, bottom, top)
            ROOT.gPad.SetFixedAspectRatio()
        else:
            n_skipped += 1
            continue

        obj.SetLineWidth(2)
        obj.Draw(draw_opt)
        out_name = sanitize_filename(full_name) + ".png"
        c.SaveAs(os.path.join(args.outdir, out_name))
        n_saved += 1

    f.Close()
    print(f"Saved {n_saved} plots to: {os.path.abspath(args.outdir)}")
    print(f"Skipped {n_skipped} objects (non-histogram types)")


if __name__ == "__main__":
    main()
#!/usr/bin/env python3
import argparse
import os
import ROOT

DEFAULT_OUTDIR = "/eos/home-j/jusong/www/iRPC/3024"


def walk_directory(root_dir, prefix=""):
    """Yield (full_name, TObject) recursively for all objects under a TDirectory."""
    for key in root_dir.GetListOfKeys():
        name = key.GetName()
        obj = key.ReadObj()
        full_name = f"{prefix}/{name}" if prefix else name
        if obj.InheritsFrom("TDirectory"):
            yield from walk_directory(obj, full_name)
        else:
            yield full_name, obj


def sanitize_filename(name):
    return name.replace("/", "__")


def main():
    parser = argparse.ArgumentParser(
        description="Export histograms from rechit_hist.root to PNG."
    )
    parser.add_argument(
        "--input",
        default="rechit_hist.root",
        help="Input ROOT file path (default: rechit_hist.root)",
    )
    parser.add_argument(
        "--outdir",
        default=DEFAULT_OUTDIR,
        help=f"Output directory for PNG files (default: {DEFAULT_OUTDIR})",
    )
    parser.add_argument(
        "--size",
        type=int,
        default=900,
        help="Canvas width/height in pixels (square canvas, default: 900).",
    )
    args = parser.parse_args()

    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(1110)

    in_path = os.path.abspath(args.input)
    if not os.path.exists(in_path):
        raise FileNotFoundError(f"Input file not found: {in_path}")

    os.makedirs(args.outdir, exist_ok=True)

    f = ROOT.TFile.Open(in_path)
    if not f or f.IsZombie():
        raise RuntimeError(f"Failed to open ROOT file: {in_path}")

    n_saved = 0
    n_skipped_type = 0

    for full_name, obj in walk_directory(f):
        is_h1 = obj.InheritsFrom("TH1") and (not obj.InheritsFrom("TH2"))
        is_h2 = obj.InheritsFrom("TH2")

        if is_h1:
            draw_opt = "hist"
        elif is_h2:
            draw_opt = "colz"
        else:
            n_skipped_type += 1
            continue

        canvas_name = "c_" + sanitize_filename(full_name)
        # Keep the distribution frame square for TH2 while still showing a color bar.
        if is_h2:
            left = 0.12
            right = 0.16
            bottom = 0.12
            top = 0.08
            frame_h = args.size
            canvas_h = int(round(frame_h / (1.0 - top - bottom)))
            canvas_w = int(round(frame_h / (1.0 - left - right)))
            c = ROOT.TCanvas(canvas_name, canvas_name, canvas_w, canvas_h)
            c.SetMargin(left, right, bottom, top)
            ROOT.gPad.SetFixedAspectRatio()
        else:
            c = ROOT.TCanvas(canvas_name, canvas_name, args.size, args.size)
            c.SetMargin(0.12, 0.05, 0.12, 0.08)

        obj.SetLineWidth(2)
        obj.Draw(draw_opt)

        out_name = sanitize_filename(full_name) + ".png"
        out_path = os.path.join(args.outdir, out_name)
        c.SaveAs(out_path)
        n_saved += 1

    f.Close()
    print(f"Saved {n_saved} plots to: {os.path.abspath(args.outdir)}")
    print(f"Skipped {n_skipped_type} objects (non-histogram types)")


if __name__ == "__main__":
    main()
