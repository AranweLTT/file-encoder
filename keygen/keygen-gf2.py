import numpy as np

# ----------------------
# GF(2) helpers (8x8)
# ----------------------
def random_byte_vector(rng=None) -> np.ndarray:
    if rng is None:
        rng = np.random
    x = rng.randint(0, 256, dtype=np.uint16) # single value
    return byte_to_vec(int(x) & 0xFF)

def byte_to_vec(b: int) -> np.ndarray:
    """Convert 0..255 integer to length-8 column vector over GF(2) (uint8)."""
    return np.array([ (b >> i) & 1 for i in reversed(range(8)) ], dtype=np.uint8).reshape(8, 1)

def vec_to_byte(v: np.ndarray) -> int:
    """Convert 8x1 vector (0/1) to integer 0..255. v expected shape (8,1) or (8,)."""
    v = v.reshape(-1)
    val = 0
    for bit in v:
        val = (val << 1) | int(bit & 1)
    return val

def mat_mult(A: np.ndarray, B: np.ndarray) -> np.ndarray:
    """Matrix multiplication over GF(2): (A @ B) % 2"""
    return (A.dot(B) % 2).astype(np.uint8)

def mat_identity() -> np.ndarray:
    return np.eye(8, dtype=np.uint8)

# ----------------------
# Inversion over GF(2)
# ----------------------
def gf2_inv(A: np.ndarray) -> np.ndarray:
    """Gauss-Jordan inverse over GF(2). A must be 8x8 and invertible (0/1 uint8)."""
    A = A.copy().astype(np.uint8)
    n = 8
    Iy = np.eye(n, dtype=np.uint8)
    # Augmented matrix [A | Iy]
    aug = np.concatenate([A, Iy], axis=1)
    row = 0
    for col in range(n):
        # find pivot with a 1 in this column
        pivot = None
        for r in range(row, n):
            if aug[r, col] == 1:
                pivot = r
                break
        if pivot is None:
            raise ValueError("Matrix not invertible over GF(2)")
        # swap
        if pivot != row:
            aug[[pivot, row]] = aug[[row, pivot]]
        # eliminate other rows
        for r in range(n):
            if r != row and aug[r, col] == 1:
                aug[r] ^= aug[row] # XOR row
        row += 1
    inv = aug[:, n:]
    return inv.astype(np.uint8)

def is_invertible(A: np.ndarray) -> bool:
    try:
        _ = gf2_inv(A)
        return True
    except ValueError:
        return False

# ----------------------
# Matrix / byte conversion
# ----------------------
def mat_to_hex(M: np.ndarray) -> str:
    """Serialize 8x8 matrix to 8-byte hex string. Each row -> one byte (MSB = leftmost element)."""
    rows = []
    for r in range(8):
        val = 0
        for bit in range(8):
            val = (val << 1) | int(M[r, bit] & 1)
        rows.append(val)
    return ''.join(f"{b:02x}" for b in rows)

def hex_to_mat(h: str) -> np.ndarray:
    """Reverse of mat_to_hex. Expects 16-hex-digit string."""
    if len(h) != 16:
        raise ValueError("public hex must be 16 hex chars (8 bytes)")
    arr = []
    for i in range(0, 16, 2):
        byte = int(h[i:i+2], 16)
        arr.append([(byte >> shift) & 1 for shift in reversed(range(8))])
    return np.array(arr, dtype=np.uint8)

# ----------------------
# Key generation
# ----------------------
def random_invertible_matrix(rng=None) -> np.ndarray:
    if rng is None:
        rng = np.random
    while True:
        M = rng.randint(0, 2, size=(8,8), dtype=np.uint8)
        if is_invertible(M):
            return M

# Choose a fixed public generator G (8x8 invertible). We'll pick one deterministically
def fixed_G():
    # Deterministic, but nontrivial invertible matrix (you can change these bits)
    # Here we pick a matrix with a simple pattern but ensure it's invertible.
    G = np.array([
        [1,0,1,0,0,1,0,1],
        [0,1,1,0,1,0,1,0],
        [1,1,0,1,0,0,1,0],
        [0,0,1,1,1,0,0,1],
        [1,0,0,1,0,0,1,0],
        [0,1,0,0,1,1,0,1],
        [1,0,1,0,1,0,1,0],
        [0,1,0,1,0,1,0,1],
    ], dtype=np.uint8)
    if not is_invertible(G):
        raise RuntimeError("Chosen G is not invertible")
    return G, gf2_inv(G)

G, G_inv = fixed_G()

def generate_keypair(rng=None):
    """Return (public_hex, private_dict). private_dict contains S and S_inv as matrices."""
    if rng is None:
        rng = np.random
    S = random_invertible_matrix(rng)
    S_inv = gf2_inv(S)
    P = mat_mult(mat_mult(S, G), S_inv) # P = S * G * S_inv
    pubhex = mat_to_hex(P)
    privhex = mat_to_hex(S)
    return pubhex, privhex

# ----------------------
# Encrypt / Decrypt (single-byte plaintext)
# ----------------------
def encrypt_byte(pubhex: str, pt_byte: int) -> int:
    """Encrypt one byte (0..255) using public key matrix (8x8). Returns ciphertext byte."""
    P = hex_to_mat(pubhex)
    p = byte_to_vec(pt_byte)
    c = mat_mult(P, p)
    return vec_to_byte(c)

def decrypt_byte(privhex: str, ct_byte: int) -> int:
    """Decrypt one byte with private key matrix (8x8). """
    S = hex_to_mat(privhex)
    S_inv = gf2_inv(S)
    c = byte_to_vec(ct_byte)
    # y = S_inv @ c
    y = mat_mult(S_inv, c)
    # u = G_inv @ y
    u = mat_mult(G_inv, y)
    # original p = S @ u
    p = mat_mult(S, u)
    return vec_to_byte(p)

# ----------------------
# Vanity public key generator
# ----------------------
def find_vanity_key(prefix_hex: str, max_tries: int = 10_000_000, progress_every: int = 1000, rng=None):
    """
    Search for a keypair where public_hex startswith prefix_hex (lowercase).
    Returns (pubhex, priv, tries).
    """
    if rng is None:
        rng = np.random
    prefix_hex = prefix_hex.lower()
    tries = 0
    while tries < max_tries:
        tries += 1
        pubhex, priv = generate_keypair(rng)
        if pubhex.startswith(prefix_hex):
            return pubhex, priv, tries
        if (tries % progress_every) == 0:
            # brief progress print
            print(f"tries={tries} (no match yet), last pub={pubhex}")
    raise RuntimeError(f"Not found after {max_tries} tries")

# ----------------------
# Example usage
# ----------------------
if __name__ == "__main__":
    np.random.seed() # system seed

    # Find a vanity key starting with 'dead' (example) -- WARNING: may take many tries
    # small prefixes like 'ab' or 'dead' are OK to try interactively; bigger ones take exponentially longer.
    print("--- Vanity key generator ---")
    pubhex, privhex, n = find_vanity_key("cafe", max_tries=200000, progress_every=10000)
    print("Found after", n)
    print("Public hex:", pubhex)
    print("Private hex:", privhex)

    # test encrypt/decrypt
    pt = 0x4f # arbitrary byte
    ct = encrypt_byte(pubhex, pt)
    pt2 = decrypt_byte(privhex, ct)
    print(f"pt=0x{pt:02x} -> ct=0x{ct:02x} -> dec=0x{pt2:02x}")
    assert pt == pt2, "roundtrip failed!"
