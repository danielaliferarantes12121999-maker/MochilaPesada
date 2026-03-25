# Mod novo (independente): Inventory Restrictions

Este mod é totalmente separado dos mods anteriores.

## Objetivo

1. Bloquear **preventivamente** munição de RPG na mochila.
2. Limitar granadas para no máximo **5** no inventário do jogador, também de forma **preventiva**.

## Estrutura sugerida

```text
<SeuMod>/
  Scripts/
    Game/
      InventoryRules/
        MP_InventoryRestrictions.c
```

## Script principal

Arquivo:

- `Scripts/Game/InventoryRules/MP_InventoryRestrictions.c`

Ele contém:

- `MP_InventoryRestrictionUtils.IsRpgAmmo(...)` → identifica munição de RPG.
- `MP_BackpackRestrictedStorageComponent.CanStoreItem(...)` → bloqueia RPG quando o destino é mochila.
- `MP_InventoryRestrictionUtils.CountGrenadesInPlayerInventory(...)` → conta granadas atuais.
- `SCR_CharacterInventoryStorageComponent.CanStoreItem(...)` (modded) → impede pegar/mover a 6ª granada.

## Onde colocar e como ligar no Workbench

1. Copie o script para o caminho acima.
2. No prefab de backpack/storage da mochila, use `MP_BackpackRestrictedStorageComponent` no storage de destino da mochila.
3. Garanta que o mod está carregado no projeto (addon ativo).
4. Rebuild scripts no Arma Reforger Tools.

## Por que esse ponto é o mais seguro

- O bloqueio ocorre em `CanStoreItem`, que é um gate de validação de armazenamento.
- Retornar `false` impede a transferência antes dela ser concluída.
- Não é estratégia de “deixar entrar e remover depois”.

## Como testar

### Teste A — RPG na mochila

1. Abra sessão local no Workbench/Play.
2. Tenha uma munição RPG na mão/chão/caixa.
3. Tente arrastar para mochila ou mover por ação de inventário.
4. Resultado esperado: a ação falha imediatamente e o item não entra na mochila.

### Teste B — limite de granadas

1. Pegue 5 granadas (qualquer fonte: chão, arsenal, container).
2. Tente pegar/mover uma 6ª granada para o inventário do jogador.
3. Resultado esperado: a ação é bloqueada imediatamente.
4. Movimentar granadas já dentro do próprio inventário (reorganização) deve continuar permitido.

## Ajustes recomendados

- Substituir heurística por whitelist exata de prefabs:
  - RPG ammo: ResourceNames exatos das munições de RPG.
  - Grenades: ResourceNames exatos das granadas válidas.
- Se quiser limite diferente, ajustar `m_iMaxGrenades` no componente.
