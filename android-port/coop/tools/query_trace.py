#!/usr/bin/env python3
"""Discover and query running DK64 LAN co-op v0.48 peers."""

import argparse
import ipaddress
import json
import select
import socket
import time

REQUEST = b"DK64COOP_TRACE_V1"


def local_broadcasts():
    targets = {"255.255.255.255"}
    candidates = set(socket.gethostbyname_ex(socket.gethostname())[2])
    probe = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        probe.connect(("192.0.2.1", 9))
        candidates.add(probe.getsockname()[0])
    except OSError:
        pass
    finally:
        probe.close()
    for address in candidates:
        try:
            ip = ipaddress.IPv4Address(address)
            if ip.is_private and not ip.is_loopback:
                targets.add(str(ipaddress.IPv4Network(f"{ip}/24", strict=False).broadcast_address))
        except ipaddress.AddressValueError:
            pass
    return sorted(targets)


def query(targets, ports, timeout):
    sockets = []
    for target in targets:
        for port in ports:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            if hasattr(socket, "SIO_UDP_CONNRESET"):
                sock.ioctl(socket.SIO_UDP_CONNRESET, False)
            sock.bind(("", 0))
            sock.sendto(REQUEST, (target, port))
            sockets.append(sock)
    deadline = time.monotonic() + timeout
    replies = {}
    while sockets and time.monotonic() < deadline:
        readable, _, _ = select.select(sockets, [], [], max(0.01, deadline - time.monotonic()))
        if not readable:
            break
        for sock in readable:
            try:
                payload, source = sock.recvfrom(8192)
            except ConnectionResetError:
                sockets.remove(sock)
                sock.close()
                continue
            try:
                data = json.loads(payload.decode("utf-8"))
            except (UnicodeDecodeError, json.JSONDecodeError):
                continue
            data["reply_ip"] = source[0]
            replies[(source[0], data.get("role", ""))] = data
    for sock in sockets:
        sock.close()
    return list(replies.values())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ip", action="append", default=[], help="query one peer directly; may be repeated")
    parser.add_argument("--port", type=int, action="append", default=[],
                        help="trace UDP port; may be repeated (default: 6465 through 6472)")
    parser.add_argument("--timeout", type=float, default=1.5, help="reply window in seconds")
    parser.add_argument("--watch", type=float, default=0, metavar="SECONDS", help="rediscover repeatedly")
    args = parser.parse_args()
    ports = args.port or list(range(6465, 6473))
    if any(port < 1 or port > 65535 for port in ports) or args.timeout <= 0 or args.watch < 0:
        parser.error("invalid port or timing value")
    targets = args.ip or local_broadcasts()
    while True:
        replies = query(targets if args.ip else local_broadcasts(), ports, args.timeout)
        if replies:
            for reply in sorted(replies, key=lambda row: (row.get("role", ""), row["reply_ip"])):
                print(json.dumps(reply, indent=2, sort_keys=True))
        else:
            print("No v0.48 peers replied. Check that the game is running and UDP traffic is allowed.")
        if not args.watch:
            break
        time.sleep(args.watch)


if __name__ == "__main__":
    main()
