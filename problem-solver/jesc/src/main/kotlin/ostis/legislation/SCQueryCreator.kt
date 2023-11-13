package ostis.legislation

import net.ostis.jesc.client.model.element.ScContentType
import net.ostis.jesc.client.model.element.ScReference
import net.ostis.jesc.client.model.type.ScType
import net.ostis.jesc.context.ScContext
import ostis.legislation.thirdparty.witai.WitAI

interface SCQueryCreator {
    fun createQuery(naturalQuery: String)
}

class WitAISCQueryCreator(
    private val ctx: ScContext,
    private val witAI: WitAI,
    private val telegramChatId: Long
): SCQueryCreator {

    private val nrelMainIdtf = ctx.findBySystemIdentifier("nrel_main_idtf").get()
    private val langRu = ctx.findBySystemIdentifier("lang_ru").get()

    private val questionNaturalLangAddr =
        ctx.resolveBySystemIdentifier("question_natural_lang", ScType.NODE_CONST_CLASS)
    private val rrelTelegramChatId =
        ctx.resolveBySystemIdentifier("rrel_telegram_chat_id", ScType.NODE_CONST_ROLE)
    private val rrelIntent =
        ctx.resolveBySystemIdentifier("rrel_intent", ScType.NODE_CONST_ROLE)
    private val rrelFirstArgument =
        ctx.resolveBySystemIdentifier("rrel_first_argument", ScType.NODE_CONST_ROLE)

    private val intentWhatIs =
        ctx.resolveBySystemIdentifier("intent_what_is", ScType.NODE_CONST)

    override fun createQuery(naturalQuery: String) {
        val witAIResult = witAI.process(naturalQuery)

        ctx.api.createElements()
            .node(ScType.NODE_CONST_CLASS) // 0

            .link(ScType.LINK_CONST, witAIResult.entities["term:term"]!![0].value, ScContentType.STRING) // 1
            .link(ScType.LINK_CONST, telegramChatId, ScContentType.INT)                                  // 2

            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.ref(0), ScReference.addr(intentWhatIs)) // 3
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.ref(0), ScReference.ref(1))       // 4
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.ref(0), ScReference.ref(2))       // 5

            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.addr(rrelIntent), ScReference.ref(3))        // 6
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.addr(rrelFirstArgument), ScReference.ref(4)) // 7
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.addr(rrelTelegramChatId), ScReference.ref(5))// 8
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.addr(questionNaturalLangAddr), ScReference.ref(0)) // 9

            .link(ScType.LINK_CONST, "Запрос на естественном языке: $naturalQuery", ScContentType.STRING) // 10
            .edge(ScType.EDGE_D_COMMON_CONST, ScReference.ref(0), ScReference.ref(10)) // 11
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.addr(nrelMainIdtf), ScReference.ref(11)) // 12
            .edge(ScType.EDGE_ACCESS_CONST_POS_PERM, ScReference.addr(langRu), ScReference.ref(10))

            .execute()
    }

}
