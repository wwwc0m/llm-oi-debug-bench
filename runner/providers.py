"""各家 LLM API 的统一封装（全部走 OpenAI 兼容协议 + base_url）。"""
import os

from dotenv import load_dotenv

load_dotenv()

# provider -> (base_url, api_key 环境变量名, 默认模型)
PROVIDERS = {
    # 智谱 GLM Coding Plan 订阅端点（区别于按量付费的 /api/paas/v4）
    "zhipu": ("https://open.bigmodel.cn/api/coding/paas/v4", "ZHIPU_API_KEY", "glm-5.3"),
    "deepseek": ("https://api.deepseek.com", "DEEPSEEK_API_KEY", "deepseek-v4-pro"),
}

# 实验默认被试（BENCH_MODELS 可覆盖）；GLM 系因思考不可控暂停参与，注册保留
DEFAULT_MODELS = [
    "deepseek:deepseek-flash",
]


def list_models():
    """返回可用的模型标识列表（形如 'deepseek:deepseek-chat'）。"""
    override = os.environ.get("BENCH_MODELS")
    if override:
        return [m.strip() for m in override.split(",") if m.strip()]
    out = []
    for spec in DEFAULT_MODELS:
        prov = spec.partition(":")[0]
        _, env, _ = PROVIDERS[prov]
        if os.environ.get(env):
            out.append(spec)
    return out


def make_client(model_spec: str):
    """model_spec 形如 'deepseek:deepseek-v4-pro' 或 'zhipu:自定义模型名'。"""
    from openai import OpenAI
    prov, _, model = model_spec.partition(":")
    if prov not in PROVIDERS:
        raise ValueError(f"未知 provider：{prov}；可选：{list(PROVIDERS)}")
    base_url, env, default = PROVIDERS[prov]
    api_key = os.environ.get(env)
    if not api_key:
        raise ValueError(f"环境变量 {env} 未设置（检查 .env）")
    client = OpenAI(api_key=api_key, base_url=base_url,
                    timeout=420, max_retries=1)  # 单次调用 7 分钟上限，失败重试 1 次
    return client, model or default
