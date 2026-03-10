# Bootloader x86 → x86_64

Bootloader minimal en deux étapes écrit en assembleur x86/x86_64 (NASM). Il amorce une machine en mode réel 16-bit et la conduit jusqu'en mode long 64-bit avant de sauter au noyau.

---

## Architecture générale

```
Disque                             Mémoire après chargement
┌─────────────────────┐            ┌─────────────────────────────┐
│ Secteur 1           │  ──────▶   │ 0x7C00  Stage 1 (512 B)     │
│ bootloader.asm      │            ├─────────────────────────────┤
├─────────────────────┤            │ 0x7E00  Stage 2 (2560 B)    │
│ Secteurs 2→6        │  ──────▶   │         (bootloaderstage2)  │
│ bootloaderstage2.asm│            ├─────────────────────────────┤
├─────────────────────┤            │ 0x10000 Kernel (10 secteurs)│
│ Secteurs 7→16       │  ──────▶   ├─────────────────────────────┤
│ Kernel              │            │ 0x1000  PML4                 │
└─────────────────────┘            │ 0x2000  PDPT                 │
                                   │ 0x3000  PD                   │
                                   │ 0x4000  PT  (identity map)   │
                                   ├─────────────────────────────┤
                                   │ 0x5FF8  Nb entrées E820      │
                                   │ 0x6000  Memory map E820      │
                                   ├─────────────────────────────┤
                                   │ 0x90000 Stack                │
                                   └─────────────────────────────┘
```

---

## Fichiers

### `bootloader.asm` — Stage 1 (512 octets)

Chargé par le BIOS à `0x7C00`. Tient dans exactement **1 secteur** (512 octets) et a pour seul rôle de charger le stage 2.

**Séquence d'exécution :**

1. **Activation de la ligne A20** via le port `0x92` (méthode Fast A20) — permet d'adresser au-delà de 1 Mo.
2. **Initialisation des segments** (`DS`, `ES` = 0) pour un adressage flat en mode réel.
3. **Chargement du stage 2** via l'interruption BIOS `INT 13h / AH=02h` :
   - 5 secteurs lus depuis le cylindre 0, tête 0, secteur 2
   - Chargés à `0x7E00` (juste après le stage 1 en mémoire)
4. **Saut vers** `0x0000:0x7E00` pour transférer le contrôle au stage 2.
5. En cas d'erreur disque : affichage du message `"Disk error!"` via `INT 10h` puis `HLT`.

**Format du secteur de boot :**
- Rembourrage avec `0x00` jusqu'à l'octet 510
- Signature de boot `0xAA55` aux octets 511-512

---

### `bootloaderstage2.asm` — Stage 2 (2560 octets)

Chargé à `0x7E00`. Occupe **5 secteurs** et orchestre toute la transition jusqu'en mode long 64-bit.

**Séquence d'exécution :**

#### 1. Chargement du noyau (mode réel 16-bit)
- Lecture de **10 secteurs** depuis le secteur 7 (cylindre 0, tête 0)
- Chargement à l'adresse `0x10000`
- Utilise `INT 13h / AH=02h` — doit être fait **avant** de quitter le mode réel

#### 2. Détection de la mémoire (INT 15h / E820)
- Appels successifs à `INT 15h / EAX=0xE820` pour énumérer les régions mémoire
- Chaque entrée fait 24 octets et est stockée à partir de `0x6000`
- Le nombre d'entrées est écrit à l'adresse `0x5FF8` pour le noyau

#### 3. GDT 32-bit et passage en mode protégé
- Définition d'une GDT minimale :
  - Descripteur nul (entrée 0)
  - Segment de code 32-bit (`0x08`) : base 0, limite 4 Go, exécutable
  - Segment de données 32-bit (`0x10`) : base 0, limite 4 Go, lecture/écriture
- `LGDT`, passage du bit `PE` dans `CR0`, far jump vers `protected_mode`

#### 4. Configuration des tables de pages 4 niveaux (mode protégé 32-bit)
- Effacement de `0x1000` à `0x5000` (16 Ko)
- Construction d'une hiérarchie de pages pour l'**identity mapping** des 2 premiers Mo :
  - `PML4[0]` → `0x2003` (PDPT, présent + R/W)
  - `PDPT[0]` → `0x3003` (PD, présent + R/W)
  - `PD[0]`   → `0x4003` (PT, présent + R/W)
  - PT : 512 entrées mappant `0x0` → `0x1FF000` (pages de 4 Ko)
- `CR3` ← `0x1000`
- Activation du **PAE** (bit 5 de `CR4`)
- Activation du **Long Mode** via le bit LME du registre MSR `0xC0000080` (`EFER`)
- Activation de la **pagination** (bit 31 de `CR0`)

#### 5. GDT 64-bit et saut en mode long
- Chargement d'une nouvelle GDT 64-bit :
  - Descripteur nul
  - Segment de code 64-bit (`0x00209A0000000000`)
  - Segment de données 64-bit (`0x0000920000000000`)
- Far jump vers `long_mode`

#### 6. Mode long 64-bit
- Initialisation de tous les segments (`DS`, `ES`, `FS`, `GS`, `SS`)
- Stack à `0x90000`
- Saut absolu vers le noyau à `0x10000`

---

## Disposition mémoire récapitulative

| Adresse   | Contenu                              | Taille       |
|-----------|--------------------------------------|--------------|
| `0x1000`  | PML4 (table de pages niveau 4)       | 4 Ko         |
| `0x2000`  | PDPT (table de pages niveau 3)       | 4 Ko         |
| `0x3000`  | PD   (table de pages niveau 2)       | 4 Ko         |
| `0x4000`  | PT   (table de pages niveau 1)       | 4 Ko         |
| `0x5FF8`  | Nombre d'entrées E820 (word)         | 2 octets     |
| `0x6000`  | Table mémoire E820                   | variable     |
| `0x7C00`  | Stage 1                              | 512 octets   |
| `0x7E00`  | Stage 2                              | 2560 octets  |
| `0x10000` | Noyau                                | ≤10 secteurs |
| `0x90000` | Stack                                | —            |

---

## Prérequis & Build

### Assemblage

```bash
# Assembler les deux fichiers
nasm -f bin bootloader.asm    -o stage1.bin
nasm -f bin bootloaderstage2.asm -o stage2.bin
```

### Création de l'image disque

```bash
# Construire une image brute (stage1 + stage2 + kernel)
cat stage1.bin stage2.bin kernel.bin > boot.img

# Optionnel : padder à une taille fixe (ex: 1.44 Mo pour une disquette)
truncate -s 1474560 boot.img
```

### Exécution avec QEMU

```bash
# Boot direct sur l'image
qemu-system-x86_64 -drive format=raw,file=boot.img

# Avec debug GDB
qemu-system-x86_64 -drive format=raw,file=boot.img -s -S &
gdb -ex "target remote :1234" -ex "set architecture i8086"
```

---

## Limitations connues

- **Méthode Fast A20** : La méthode port `0x92` n'est pas supportée par tous les firmwares/émulateurs. Pour une compatibilité maximale, envisager la méthode BIOS (`INT 15h / AX=2401h`) ou via le contrôleur clavier (port `0x64`/`0x60`).
- **INT 13h CHS** : Le chargement disque utilise l'adressage CHS (Cylindre/Tête/Secteur). La limite théorique est de ~8 Go, mais la robustesse est meilleure avec LBA (mode étendu `AH=42h`).
- **Pas de gestion d'erreur en stage 2** : En cas d'échec disque, le CPU fait simplement `HLT` sans message d'erreur.
- **Identity mapping limité à 2 Mo** : Une seule PT de 512 entrées. Le noyau devra étendre les tables de pages.
- **GDT 64-bit en mémoire basse** : La `gdt64_descriptor` utilise une adresse 32-bit (`dd`). Acceptable en phase de boot mais à relocaliser si le noyau libère la mémoire basse.

---

## Chaîne de transition des modes CPU

```
BIOS (16-bit réel)
      │
      ▼
Stage 1 — 0x7C00 (16-bit réel)
  • A20, init segments, load stage 2
      │
      ▼
Stage 2 — 0x7E00 (16-bit réel)
  • Load kernel, E820 memory map
  • GDT 32-bit → CR0.PE = 1
      │
      ▼
Protected Mode (32-bit)
  • Page tables (PML4/PDPT/PD/PT)
  • CR3, PAE, EFER.LME, CR0.PG
  • GDT 64-bit → far jump
      │
      ▼
Long Mode (64-bit)
  • Init segments, stack
  • JMP 0x10000 → Kernel
```