#!/usr/bin/env python3
"""Download completed daily Binance spot candles in CSV format."""

from __future__ import annotations

import argparse
import csv
import json
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from datetime import datetime, timezone
from pathlib import Path


API_URL = "https://api.binance.com/api/v3/klines"
DAY_MS = 86_400_000
LIMIT = 1000
HEADER = [
    "open_time",
    "open",
    "high",
    "low",
    "close",
    "volume",
    "close_time",
    "quote_asset_volume",
    "number_of_trades",
    "taker_buy_base_asset_volume",
    "taker_buy_quote_asset_volume",
    "ignore",
]


def date_ms(value: str) -> int:
    try:
        value_as_date = datetime.strptime(value, "%Y-%m-%d")
    except ValueError as error:
        raise argparse.ArgumentTypeError("dates must use YYYY-MM-DD") from error
    return int(value_as_date.replace(tzinfo=timezone.utc).timestamp() * 1000)


def fetch(symbol: str, start_ms: int, end_ms: int) -> list[list]:
    params = urllib.parse.urlencode(
        {
            "symbol": symbol,
            "interval": "1d",
            "startTime": start_ms,
            "endTime": end_ms,
            "limit": LIMIT,
        }
    )
    request = urllib.request.Request(
        f"{API_URL}?{params}",
        headers={"User-Agent": "event-driven-backtester/1.0"},
    )

    for attempt in range(5):
        try:
            with urllib.request.urlopen(request, timeout=30) as response:
                result = json.load(response)
            if not isinstance(result, list):
                raise RuntimeError(f"unexpected Binance response: {result}")
            return result
        except urllib.error.HTTPError as error:
            if error.code not in {418, 429, 500, 502, 503, 504} or attempt == 4:
                message = error.read().decode("utf-8", errors="replace")
                raise RuntimeError(f"Binance returned HTTP {error.code}: {message}") from error
        except urllib.error.URLError as error:
            if attempt == 4:
                raise RuntimeError(f"could not reach Binance: {error.reason}") from error
        time.sleep(2**attempt)

    return []


def download(symbol: str, start_ms: int, end_ms: int) -> list[list]:
    candles: list[list] = []
    next_start = start_ms

    while next_start <= end_ms:
        batch = fetch(symbol, next_start, end_ms)
        if not batch:
            break

        candles.extend(batch)
        next_start = int(batch[-1][0]) + DAY_MS
        print(f"Downloaded {len(candles)} candles", file=sys.stderr)

        if len(batch) < LIMIT:
            break

    return candles


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--symbol", default="BTCUSDT")
    parser.add_argument("--start", type=date_ms, default=date_ms("2021-06-19"))
    parser.add_argument("--end", type=date_ms, default=date_ms("2026-06-18"))
    parser.add_argument(
        "--output", type=Path, default=Path("Data/btcusdt_1d_5y.csv")
    )
    args = parser.parse_args()

    if args.end < args.start:
        parser.error("--end must not be earlier than --start")

    try:
        candles = download(args.symbol.upper(), args.start, args.end)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        with args.output.open("w", newline="", encoding="utf-8") as output_file:
            writer = csv.writer(output_file)
            writer.writerow(HEADER)
            writer.writerows(candles)
    except (OSError, RuntimeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"Wrote {len(candles)} candles to {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
