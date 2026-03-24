# Bloquear munição de RPG na mochila (pré-transferência)

## Estratégia correta (preventiva)

No Arma Reforger, o ponto mais limpo para impedir a transferência **antes** de acontecer é no método de validação de armazenamento do próprio storage de destino:

- `BaseInventoryStorageComponent.CanStoreItem(IEntity item)`
- habilitando override dinâmico com `OnOverrideCanStoreItem() => true`

Com isso, quando o destino for a mochila, a operação de arrastar/mover/guardar é barrada no check de permissões de storage.

## Arquivos deste exemplo

- `Scripts/Game/Inventory/MP_RestrictedBackpackStorageComponent.c`

## Como funciona

1. O componente `MP_RestrictedBackpackStorageComponent` é usado no storage da mochila.
2. `CanStoreItem` roda antes de inserir o item no container.
3. O script lê o prefab do item (`item.GetSource().GetPrefabName()`).
4. Consulta uma tabela de bloqueio por container (`MP_InventoryRestrictionDB`).
5. Se o item estiver na lista para `backpack`, retorna `false` e a transferência é cancelada.

## Modularidade (futuro)

Para adicionar mais itens/containers:

- registrar novos itens em `MP_InventoryRestrictionDB.RegisterBlockedItem("containerKey", "PrefabResourceName")`
- criar outro componente de storage com `m_sContainerKey` diferente
- ou reutilizar o mesmo componente mudando o atributo da chave no prefab do container

## Onde colocar no mod

Estrutura recomendada:

- `<SeuMod>/Scripts/Game/Inventory/MP_RestrictedBackpackStorageComponent.c`

Depois, no Workbench:

1. Abra o prefab da mochila (ou do componente de storage da mochila).
2. Troque/adicione o componente para usar `MP_RestrictedBackpackStorageComponent` no storage desejado.
3. Garanta que a chave do container está como `backpack` (ou a chave que você configurou).
4. Atualize os `ResourceName` das munições RPG para os prefabs reais do seu projeto.

## Importante

- Este método bloqueia no momento da tentativa (não deixa entrar e remover depois).
- Caso algum fluxo específico ignore `CanStoreItem` (raro), a alternativa é interceptar no manager de inventário; porém, na prática, o storage de destino é a abordagem mais direta e previsível para regra por container.
