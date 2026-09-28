"""各家 LLM API 的统一封装（全部走 OpenAI 兼容协议 + base_url）。"""
import json
import os

from dotenv import load_dotenv

load_dotenv()

# provider -> (base_url, api_key 环境变量名, 默认模型)
PROVIDERS = {
    "deepseek":  ("https://api.deepseek.com",                              "DEEPSEEK_API_KEY",  "deepseek-chat"),
    "zhipu":     ("https://open.bigmodel.cn/api/paas/v4",                  "ZHIPU_API_KEY",     "glm-4.6"),
    "gemini":    ("https://generativelanguage.googleapis.com/v1beta/openai", "GEMINI_API_KEY",  "gemini-2.5-flash"),
    "openai":    ("https://api.openai.com/v1",                             "OPENAI_API_KEY",    "gpt-4o"),
}


def list_models():
    """返回可用的模型标识列表（形如 'deepseek:deepseek-chat'）。"""
    override = os.environ.get("BENCH_MODELS")
    if override:
        return [m.strip() for m in override.split(",") if m.strip()]
    out = []
    for prov, (_, env, model) in PROVIDERS.items():
        if os.environ.get(env):
            out.append(f"{prov}:{model}")
    return out


def make_client(model_spec: str):
    """model_spec 形如 'deepseek:deepseek-chat' 或 'deepseek:自定义模型名'。"""
    from openai import OpenAI
    prov, _, model = model_spec.partition(":")
    if prov not in PROVIDERS:
        raise ValueError(f"未知 provider：{prov}；可选：{list(PROVIDERS)}")
    base_url, env, default = PROVIDERS[prov]
    api_key = os.environ.get(env)
    if not api_key:
        raise ValueError(f"环境变量 {env} 未设置（检查 .env）")
    client = OpenAI(api_key=api_key, base_url=base_url)
    return client, model or default
