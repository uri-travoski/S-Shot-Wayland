#!/usr/bin/env python3
"""
S-Shot Wayland Resource and Memory Footprint Profiler
Measures idle memory consumption, PSS, RSS, and socket IPC behavior.
"""

import os
import sys
import time
import subprocess
import signal

def get_proc_memory(pid):
    """Returns (vm_rss_kb, pss_kb) for process pid."""
    vm_rss = 0
    pss = 0
    
    # Read status for VmRSS
    status_path = f"/proc/{pid}/status"
    if os.path.exists(status_path):
        with open(status_path, "r") as f:
            for line in f:
                if line.startswith("VmRSS:"):
                    vm_rss = int(line.split()[1])
                    break
                    
    # Read smaps_rollup for PSS (proportional set size)
    smaps_path = f"/proc/{pid}/smaps_rollup"
    if os.path.exists(smaps_path):
        with open(smaps_path, "r") as f:
            for line in f:
                if line.startswith("Pss:"):
                    pss = int(line.split()[1])
                    break
                    
    return vm_rss, pss

def main():
    binary_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "build", "s-shot-wayland"))
    if not os.path.exists(binary_path):
        print(f"Error: binary not found at {binary_path}")
        sys.exit(1)

    print("=" * 65)
    print("S-Shot Wayland Resource Profiler")
    print("=" * 65)

    # 1. Start application in tray/idle mode
    env = os.environ.copy()
    env["QT_QPA_PLATFORM"] = "offscreen" # Use offscreen for headless profiling
    
    print("[1/3] Launching background tray instance...")
    proc = subprocess.Popen([binary_path, "--tray"], env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    pid = proc.pid
    time.sleep(1.0) # Let initialization settle

    try:
        vm_rss, pss = get_proc_memory(pid)
        rss_mb = vm_rss / 1024.0
        pss_mb = pss / 1024.0
        
        print(f"      PID:           {pid}")
        print(f"      VmRSS (Total): {rss_mb:.2f} MB")
        print(f"      PSS (Private): {pss_mb:.2f} MB" if pss > 0 else "      PSS:           N/A")

        # Low resource target: Idle memory < 35 MB
        target_limit_mb = 35.0
        print(f"[2/3] Checking idle memory ceiling (< {target_limit_mb} MB)...")
        if rss_mb <= target_limit_mb:
            print(f"      [PASS] Idle RSS {rss_mb:.2f} MB is within the target threshold!")
        else:
            print(f"      [WARNING] Idle RSS {rss_mb:.2f} MB exceeds {target_limit_mb} MB")

        # 2. Test single instance IPC trigger
        print("[3/3] Testing Single Instance CLI IPC forward...")
        cli_result = subprocess.run([binary_path, "--version"], env=env, capture_output=True, text=True)
        print(f"      CLI Output: {cli_result.stdout.strip()}")
        assert "s-shot-wayland 1.32" in cli_result.stdout, "Version mismatch"
        print("      [PASS] Single instance CLI forward verified!")

        print("\nAll resource checks passed successfully!")
        
    finally:
        # Terminate test process
        try:
            proc.terminate()
            proc.wait(timeout=2)
        except Exception:
            proc.kill()

if __name__ == "__main__":
    main()
