"""轨迹记录：把每轮交互追加写入 JSONL，供统计与质性分析。"""
import json
import os
import time


class Session:
    def __init__(self, out_dir: str, problem_id: str, model: str, phase: str,
                 extra: dict | None = None):
        os.makedirs(out_dir, exist_ok=True)
        ts = time.strftime("%Y%m%d-%H%M%S")
        self.path = os.path.join(out_dir, f"{problem_id}__{model.replace(':', '_')}__{phase}__{ts}.jsonl")
        self._meta = {"problem": problem_id, "model": model, "phase": phase,
                      "start": ts, **(extra or {})}
        self._append(self._meta)

    def _append(self, obj):
        with open(self.path, "a", encoding="utf-8") as f:
            f.write(json.dumps(obj, ensure_ascii=False) + "\n")

    def log_round(self, round_no: int, action: str, code: str | None,
                  tool_result: dict | None, usage: dict | None):
        self._append({"round": round_no, "action": action, "code": code,
                      "tool_result": tool_result, "usage": usage,
                      "t": time.strftime("%H:%M:%S")})

    @property
    def file(self):
        return self.path
