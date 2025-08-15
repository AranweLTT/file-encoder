import numpy as np
import sys
import os

# ----------------------
# GF(2) helpers (8x8)
# ----------------------
def byte_to_vec(b: int) -> np.ndarray:
    return np.unpackbits(np.array([b], dtype=np.uint8)).reshape(8, 1)

def vec_to_byte(v: np.ndarray) -> int:
    return int(np.packbits(v.reshape(-1))[0])

def mat_mult(A: np.ndarray, B: np.ndarray) -> np.ndarray:
    return (A.dot(B) % 2).astype(np.uint8)

def gf2_inv(A: np.ndarray) -> np.ndarray:
    A = A.copy().astype(np.uint8)
    n = 8
    Iy = np.eye(n, dtype=np.uint8)
    aug = np.concatenate([A, Iy], axis=1)
    row = 0
    for col in range(n):
        pivot = None
        for r in range(row, n):
            if aug[r, col] == 1:
                pivot = r
                break
        if pivot is None:
            raise ValueError("Matrix not invertible over GF(2)")
        if pivot != row:
            aug[[pivot, row]] = aug[[row, pivot]]
        for r in range(n):
            if r != row and aug[r, col] == 1:
                aug[r] ^= aug[row]
        row += 1
    return aug[:, n:].astype(np.uint8)

def is_invertible(A: np.ndarray) -> bool:
    try:
        _ = gf2_inv(A)
        return True
    except ValueError:
        return False

def mat_to_hex(M: np.ndarray) -> str:
    rows = []
    for r in range(8):
        val = 0
        for bit in range(8):
            val = (val << 1) | int(M[r, bit] & 1)
        rows.append(val)
    return ''.join(f"{b:02x}" for b in rows)

def hex_to_mat(h: str) -> np.ndarray:
    if len(h) != 16:
        raise ValueError("public hex must be 16 hex chars (8 bytes)")
    arr = []
    for i in range(0, 16, 2):
        byte = int(h[i:i+2], 16)
        arr.append([(byte >> shift) & 1 for shift in reversed(range(8))])
    return np.array(arr, dtype=np.uint8)

# ----------------------
# Fixed generator matrix
# ----------------------
def fixed_G(G_hex):
    G = hex_to_mat(G_hex)
    if not is_invertible(G):
        raise RuntimeError("Chosen G is not invertible")
    return G, gf2_inv(G)

G, G_inv = fixed_G("a56ad239924daa55")

# ----------------------
# Encrypt / Decrypt
# ----------------------
def encrypt_byte(pubhex: str, pt_byte: int) -> int:
    P = hex_to_mat(pubhex)
    p = byte_to_vec(pt_byte)
    c = mat_mult(P, p)
    return vec_to_byte(c)

def decrypt_byte(privhex: str, ct_byte: int) -> int:
    S = hex_to_mat(privhex)
    S_inv = gf2_inv(S)
    c = byte_to_vec(ct_byte)
    y = mat_mult(S_inv, c)
    u = mat_mult(G_inv, y)
    p = mat_mult(S, u)
    return vec_to_byte(p)

# ----------------------
# File operations
# ----------------------
def encrypt_file(infile: str, outfile: str, pubhex: str):
    with open(infile, "rb") as f_in, open(outfile, "wb") as f_out:
        while True:
            chunk = f_in.read(4096)
            if not chunk:
                break
            encrypted = bytes([encrypt_byte(pubhex, b) for b in chunk])
            f_out.write(encrypted)

def decrypt_file(infile: str, outfile: str, privhex: str):
    with open(infile, "rb") as f_in, open(outfile, "wb") as f_out:
        while True:
            chunk = f_in.read(4096)
            if not chunk:
                break
            decrypted = bytes([decrypt_byte(privhex, b) for b in chunk])
            f_out.write(decrypted)

# ----------------------
# Main CLI
# ----------------------
if __name__ == "__main__":
    if len(sys.argv) < 5:
        print(f"Usage: {sys.argv[0]} encrypt|decrypt <input> <output> <pub_or_priv_hex>")
        sys.exit(1)

    mode, infile, outfile, keyhex = sys.argv[1:5]

    if not os.path.exists(infile):
        print(f"Error: Input file '{infile}' not found.")
        sys.exit(1)

    if mode == "encrypt":
        encrypt_file(infile, outfile, keyhex)
        print(f"File encrypted to {outfile}")
    elif mode == "decrypt":
        decrypt_file(infile, outfile, keyhex)
        print(f"File decrypted to {outfile}")
    else:
        print("Mode must be 'encrypt' or 'decrypt'.")
        sys.exit(1)
